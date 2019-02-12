#include "window-select-screen-pos.hpp"
#include <QSizeGrip>
#include <QDebug>

SelectScreenPosWindow::SelectScreenPosWindow(QWidget* parent) :
	QDialog(parent),
	m_ui(new Ui::SelectScreenPos)
{
	m_isMoving = false;
    
	#ifdef __APPLE__
		setWindowFlags(Qt::Widget | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	#else
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::SubWindow);
	#endif
		
	setParent(0); // Create TopLevel-Widget
	setAttribute(Qt::WA_TranslucentBackground, true);
	m_ui->setupUi(this);
	setBackgroundRole(QPalette::NoRole);
	setText("Press Enter to Select or Escape to Cancel.");
	setAcceptMouseInput(true);
}

SelectScreenPosWindow::~SelectScreenPosWindow()
{
	delete m_ui;
}

QScreen * SelectScreenPosWindow::hostScreen() const
{
	return m_hostMonitor;
}

void SelectScreenPosWindow::setText(QString text) const
{
	m_ui->label->setText(text);
}

void SelectScreenPosWindow::setAcceptMouseInput(bool mouseInput)
{
	m_ignoreMouse = !mouseInput;
	setAttribute(Qt::WA_TransparentForMouseEvents, !mouseInput);
	m_ui->widget->setAttribute(Qt::WA_TransparentForMouseEvents, !mouseInput);
	m_ui->centerContainerWidget->setAttribute(Qt::WA_TransparentForMouseEvents, !mouseInput);
	m_ui->label->setAttribute(Qt::WA_TransparentForMouseEvents, !mouseInput);
	setSizeGripEnabled(mouseInput);
	if(mouseInput)
	{
		m_ui->centerContainerWidget->setStyleSheet(
			"border: none;"
			"background: rgba(0, 0, 0, 100);"
		);
	}
	else
	{
		m_ui->centerContainerWidget->setStyleSheet(
			"border: none;"
			"background: rgba(0, 0, 0, 0);"
		);
	}
}

void SelectScreenPosWindow::setAcceptKeyboardInput(bool keyboardInput)
{
	m_ignoreKeyboard = !keyboardInput;
}

void SelectScreenPosWindow::keyPressEvent(QKeyEvent* event)
{
	if(!m_isMoving && !m_ignoreKeyboard)
	{
		if (event->key() == Qt::Key_Escape)
		{
			reject();
		}
		else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
		{
			if (hostScreen() != nullptr)
			{
				accept();
			}
			else
			{
				reject();
			}
		}
	}
}

void SelectScreenPosWindow::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
	setupGeometry();
}

void SelectScreenPosWindow::mouseMoveEvent(QMouseEvent* event)
{
	if(event->buttons().testFlag(Qt::LeftButton) && m_isMoving)
	{
		move(pos() + (event->pos() - m_lastMousePos));
	}
}

void SelectScreenPosWindow::mousePressEvent(QMouseEvent* event)
{
	if(event->button() == Qt::LeftButton && !m_ignoreMouse)
	{
		m_isMoving = true;
		m_lastMousePos = event->pos();
	}
}

void SelectScreenPosWindow::mouseReleaseEvent(QMouseEvent* event)
{
	if(event->button() == Qt::LeftButton)
	{
		m_isMoving = false;
		setupGeometry();
	}
}


void SelectScreenPosWindow::setupGeometry()
{
	// snapping to single desktop
	int areaOfIntersection = 0;
	QScreen* mostInScreen = nullptr;
	QList<QScreen*> screens = QApplication::screens();
	for (QList<QScreen*>::iterator iter = screens.begin();
		iter != screens.end(); ++iter)
	{
		// checking if rectangle is on screen
		if ((*iter)->virtualGeometry().intersects(geometry()))
		{
			QRect screenGeo = (*iter)->geometry();
			QRectF rarea = screenGeo & geometry();
			int area = rarea.width() * rarea.height();
			if (mostInScreen == nullptr || area > areaOfIntersection)
			{
				areaOfIntersection = area;
				mostInScreen = (*iter);
			}
		}
	}

	if (mostInScreen == nullptr)
	{
		return;
	}

	QRect geo = geometry();
	QRect screenGeo = mostInScreen->geometry();
	if (geo.width() > screenGeo.width())
	{
		geo.setWidth(screenGeo.width());
	}
	if (geo.height() > screenGeo.height())
	{
		geo.setHeight(screenGeo.height());
	}
	if (geo.top() < screenGeo.top())
	{
		geo.moveTop(screenGeo.top());
	}
	if (geo.bottom() > screenGeo.bottom())
	{
		geo.moveBottom(screenGeo.bottom());
	}
	if (geo.left() < screenGeo.left())
	{
		geo.moveLeft(screenGeo.left());
	}
	if (geo.right() > screenGeo.right())
	{
		geo.moveRight(screenGeo.right());
	}

	setGeometry(geo);
	m_hostMonitor = mostInScreen;
}
