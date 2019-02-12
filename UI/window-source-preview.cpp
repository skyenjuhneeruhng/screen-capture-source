#include "window-source-preview.hpp"
#include "qt-wrappers.hpp"
#include "display-helpers.hpp"
#include <QDebug>


SourcePreviewWindow::SourcePreviewWindow(obs_sceneitem_t* item, QWidget* parent)
	: SelectScreenPosWindow(parent)
{
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_StaticContents);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_DontCreateNativeAncestors);
	setAttribute(Qt::WA_NativeWindow);
	setAttribute(Qt::WA_TranslucentBackground, false);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	setSizeGripEnabled(false);
	m_sceneitem = item;
	m_isSizing = true;
	m_shouldTriggerMoveEvent = true;
	m_shouldTriggerResizeEvent = true;
	obs_source_t* source = obs_sceneitem_get_source(m_sceneitem);
	m_display = new OBSQTDisplay(this);

	// clearing content...
	QLayoutItem* litem;
	while ((litem = layout()->takeAt(0))) {
		if (litem->widget()) {
			delete litem->widget();
		}
		delete litem;
	}

	// adding display to window...
	layout()->addWidget(m_display);

	// setting up source
	obs_source_inc_showing(source);
	auto addDrawCallback = [this]()
	{
		obs_display_add_draw_callback(
			m_display->GetDisplay(),
			&SourcePreviewWindow::DrawPreview,
			this
		);
	};
	enum obs_source_type type = obs_source_get_type(source);
	uint32_t caps = obs_source_get_output_flags(source);
	bool drawable_type = type == OBS_SOURCE_TYPE_INPUT ||
		type == OBS_SOURCE_TYPE_SCENE;

	if (drawable_type && (caps & OBS_SOURCE_VIDEO) != 0)
		connect(m_display, &OBSQTDisplay::DisplayCreated,
			addDrawCallback);

	connect(this, SIGNAL(VideoRendered()), this, SLOT(VideoRenderCallback()));
	connect(this, SIGNAL(SourceLoaded()), this, SLOT(ResizeToBounds()));
}

SourcePreviewWindow::~SourcePreviewWindow()
{
	obs_display_remove_draw_callback(
		m_display->GetDisplay(),
		&SourcePreviewWindow::DrawPreview,
		this
	);
	delete m_display;
	obs_source_t* source = obs_sceneitem_get_source(m_sceneitem);
	obs_source_dec_showing(source);
}


void SourcePreviewWindow::ResizeToBounds() {
	
	/*skyli changed*/
	QRect b = geometry();
	if (b.width() > m_windowBounds.width() || b.height() > m_windowBounds.height())
	{
		if (b.width() / m_windowBounds.width() > b.height() / m_windowBounds.height())
		{
			QSize optimalSize = QSize(
				m_windowBounds.width(),
				m_windowBounds.height()
			);
			m_shouldTriggerResizeEvent = false;
			resize(m_windowBounds.size().boundedTo(optimalSize));
		}
		else
		{
			QSize optimalSize = QSize(
				b.width() * m_windowBounds.height() / b.height(),
				m_windowBounds.height()
			);

			m_shouldTriggerResizeEvent = false;
			resize(m_windowBounds.size().boundedTo(optimalSize));
		}
	}
	if (b.x() < m_windowBounds.x())
		b.setX(m_windowBounds.x());
	if (b.y() < m_windowBounds.y())
		b.setY(m_windowBounds.y());
	if ((b.x() + b.width()) > (m_windowBounds.x() + m_windowBounds.width()))
		b.setX((m_windowBounds.x() + m_windowBounds.width()) - b.width());
	if ((b.y() + b.height()) > (m_windowBounds.y() + m_windowBounds.height()))
		b.setY((m_windowBounds.y() + m_windowBounds.height()) - b.height());
	m_isSizing = false;
	m_shouldTriggerMoveEvent = false;
	move(b.x(), b.y());
	/*skyli changed*/
}


void SourcePreviewWindow::setWindowBounds(QRect bounds)
{
	m_windowBounds = bounds;
}

void SourcePreviewWindow::DrawPreview(void* data, uint32_t cx, uint32_t cy)
{
	SourcePreviewWindow *window = static_cast<SourcePreviewWindow*>(data);

	if (!window->m_sceneitem)
		return;

	obs_source_t* source = obs_sceneitem_get_source(window->m_sceneitem);

	uint32_t sourceCX = std::max(obs_source_get_width(source), 1u);
	uint32_t sourceCY = std::max(obs_source_get_height(source), 1u);

	int   x, y;
	int   newCX, newCY;
	float scale;

	GetScaleAndCenterPos(sourceCX, sourceCY, cx, cy, x, y, scale);

	newCX = int(scale * float(sourceCX));
	newCY = int(scale * float(sourceCY));

	gs_viewport_push();
	gs_projection_push();
	gs_ortho(0.0f, float(sourceCX), 0.0f, float(sourceCY),
		-100.0f, 100.0f);
	gs_set_viewport(x, y, newCX, newCY);

	obs_source_video_render(source);

	gs_projection_pop();
	gs_viewport_pop();

	emit window->VideoRendered();
}

void SourcePreviewWindow::moveEvent(QMoveEvent* event)
{
	if(!m_shouldTriggerMoveEvent)
	{
		m_shouldTriggerMoveEvent = true;
		event->ignore();
		return;
	}

	QRect b = geometry();

	if(m_windowBounds != QRect())
	{
		
		if (b.x() < m_windowBounds.x())
			b.setX(m_windowBounds.x());
		if (b.y() < m_windowBounds.y())
			b.setY(m_windowBounds.y());
		if ((b.x() + b.width()) > (m_windowBounds.x() + m_windowBounds.width()))
			b.setX((m_windowBounds.x() + m_windowBounds.width()) - b.width());
		if ((b.y() + b.height()) > (m_windowBounds.y() + m_windowBounds.height()))
			b.setY((m_windowBounds.y() + m_windowBounds.height()) - b.height());
	}

	if(b != geometry())
	{
		event->ignore();
	}
	else
	{
		SelectScreenPosWindow::moveEvent(event);
	}

	m_shouldTriggerMoveEvent = false;
	move(b.x(), b.y());
	if(!m_isSizing)
	{
		vec2 pos;
		pos.x = b.x();
		pos.y = b.y();
		/*skyli added*/
		if(m_windowBounds != QRect())
		{
			pos.x -= m_windowBounds.x();
			pos.y -= m_windowBounds.y();
		}
		/*skyli added*/
		obs_sceneitem_set_pos(m_sceneitem, &pos);
	}
}

void SourcePreviewWindow::resizeEvent(QResizeEvent* event)
{
	if (!m_shouldTriggerResizeEvent)
	{
		m_shouldTriggerResizeEvent = true;
		event->ignore();
		return;
	}

	QRect b = geometry();

	if (m_windowBounds != QRect())
	{
		if (b.x() < m_windowBounds.x())
			b.setX(m_windowBounds.x());
		if (b.y() < m_windowBounds.y())
			b.setY(m_windowBounds.y());
		if ((b.x() + b.width()) > (m_windowBounds.x() + m_windowBounds.width()) || (b.y() + b.height()) > (m_windowBounds.y() + m_windowBounds.height()))
		{
			b.setSize(QSize(((b.x() + b.width()) > (m_windowBounds.x() + m_windowBounds.width()))?m_windowBounds.width() - b.x() + m_windowBounds.x():b.width(), ((b.y() + b.height()) > (m_windowBounds.y() + m_windowBounds.height()))? b.width(), m_windowBounds.height() - b.y() + m_windowBounds.y():b.height()));
			//b.setX((m_windowBounds.x() + m_windowBounds.width()) - b.width());
		}
			
		//if ((b.y() + b.height()) > (m_windowBounds.y() + m_windowBounds.height()))
		//{
			//b.setSize(QSize(b.width() , m_windowBounds.height() - b.y() + m_windowBounds.y()));
			//b.setY((m_windowBounds.y() + m_windowBounds.height()) - b.height());
		//}
			
	}

	if (b != geometry())
	{
		event->ignore();
	}
	else
	{
		SelectScreenPosWindow::resizeEvent(event);
	}

	m_shouldTriggerResizeEvent = false;
	resize(b.size());
	m_shouldTriggerMoveEvent = false;
	move(b.x(), b.y());

	

	if(!m_isSizing)
	{
		QRect geom = geometry();
		uint32_t bw, bh;
		vec2 pos, scale;
		obs_source_t* src = obs_sceneitem_get_source(m_sceneitem);
		bw = obs_source_get_base_width(src);
		bh = obs_source_get_base_height(src);
		scale.x = (float)geom.width() / bw;
		scale.y = scale.x;
		pos.x = geom.x();
		pos.y = geom.y();
		
		qDebug() << "resizeEvent" << bw << bh << "to" << geometry() << "=" << scale.x;

		/*skyli added*/
		if(m_windowBounds != QRect())
		{
			pos.x -= m_windowBounds.x();
			pos.y -= m_windowBounds.y();
		}
		/*skyli added*/
		obs_sceneitem_set_scale(m_sceneitem, &scale);
		obs_sceneitem_set_pos(m_sceneitem, &pos);
	}
}

void SourcePreviewWindow::VideoRenderCallback()
{
	// resizing
	uint32_t w, h;
	vec2 pos, scale;
	obs_source_t* src = obs_sceneitem_get_source(m_sceneitem);
	w = obs_source_get_width(src);
	h = obs_source_get_height(src);
	obs_sceneitem_get_pos(m_sceneitem, &pos);
	obs_sceneitem_get_scale(m_sceneitem, &scale);
	m_isSizing = true;
	/*skyli added*/
	if(m_windowBounds != QRect())
	{
		pos.x += m_windowBounds.x();
		pos.y += m_windowBounds.y();
	}
	/*skyli added*/
	setGeometry(pos.x, pos.y, w * scale.x, h * scale.y);
	m_isSizing = false;
	
	if (shouldResizeToBounds && w > 0) { 
		shouldResizeToBounds = false;
		emit(SourceLoaded());
	}
}
