#include "video-item-widget.hpp"
#include "confirm-deletion.hpp"
#include "features/custom-widgets/video-file.hpp"
#include "features/upload/file-uploader.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMediaPlaylist>
#include <QSpacerItem>
#include <QFileInfo>
#include <QMessageBox>
#include <QNetworkReply>
#include <QtWidgets>

#include <QtCore/QStandardPaths>

VideoItemWidget::VideoItemWidget(QString path, QWidget* parent) : QWidget(parent)
{
	SetPath(path);
	SetupUI();
	
	setMinimumHeight(125);
	setMaximumHeight(170);
	setMaximumWidth(thumbnailSize().width() + 50);

	connect(m_edit, SIGNAL(clicked()), this, SLOT(EditButtonClicked()));//client requested to remove
	connect(m_upload, SIGNAL(clicked()), this, SLOT(UploadButtonClicked()));//client requested to remove
	connect(m_delete, SIGNAL(clicked()), this, SLOT(DeleteButtonClicked()));
	//skyli added
	connect(m_image, SIGNAL(clicked()), this, SLOT(onImageBtnClicked()));
}

VideoItemWidget::~VideoItemWidget()
{

}

void VideoItemWidget::SetPath(QString path)
{
	m_path = path;
}


QString VideoItemWidget::filePath() {
	return m_path;
}

QString VideoItemWidget::name() {
	return QFileInfo(filePath()).fileName();
}

QSize VideoItemWidget::thumbnailSize() {
	return QSize(150, 100);
}


void VideoItemWidget::EditButtonClicked()
{

	emit EditClicked(m_path);//skyli added
	// EditingWindow *editingWindow = new EditingWindow(filePath());
	// editingWindow->show();//skyli changed
}

void VideoItemWidget::UploadButtonClicked()
{
	uploadFile();
}

void VideoItemWidget::DeleteButtonClicked()
{
	if (!confirmDeletion()) return;
	
	eraseFile();
	emit FileErased();
}


void VideoItemWidget::eraseFile()
{
	QFile(filePath()).remove();
}

void VideoItemWidget::uploadFile()
{
	FileUploader* uploader = new FileUploader(filePath());
	if (uploader->canUpload()) { 
		connect(uploader, SIGNAL(Finished(QNetworkReply*)), this, SLOT(UploadFinished(QNetworkReply*)));
		uploader->perform();
		
		uploadStarted();
	} else {
		(new QMessageBox(QMessageBox::Icon::Critical, "Uploads disabled", "The encrypted configuration file could not be read."))->exec();
	}
}


void VideoItemWidget::uploadStarted() {
	m_edit->hide();//client requested to remove
	m_upload->hide();//client requested to remove
	m_delete->hide();

	uploadIndicator->setText("Uploading...");
	uploadIndicator->setStyleSheet("color: gray; font-variant: italic;");
	uploadIndicator->show();
}

void VideoItemWidget::UploadFinished(QNetworkReply* reply) {

	if (reply->error()) {
		(new QMessageBox(QMessageBox::Icon::Warning, "Upload failed", reply->errorString()))->exec();

		m_edit->show();//client requested to remove
		m_upload->show();//client requested to remove
		m_delete->show();
		uploadIndicator->hide();
	} else {
		uploadIndicator->setText("Success");
		uploadIndicator->setStyleSheet("color: green;");
	}
}


void VideoItemWidget::SetupUI()
{
	m_image = new QToolButton;
	m_edit = new QToolButton;//client requested to remove
	m_upload = new QToolButton;//client requested to remove
	m_delete = new QToolButton;

	m_nameLabel = new QLabel(name());	

	m_nameLabel->setWordWrap(true);
	m_nameLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	m_nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	m_nameLabel->setMaximumWidth(210);

	m_selectionCheckbox = new WhiteCheckbox(this);

	m_image->setObjectName("imageLabel");
	m_selectionCheckbox->setObjectName("selectionCheckbox");
	m_nameLabel->setObjectName("nameLabel");
	m_edit->setObjectName("edit");//client requested to remove
	m_upload->setObjectName("upload");//client requested to remove
	m_delete->setObjectName("delete");
	
	setThumbnail();


	QVBoxLayout* vlayout = new QVBoxLayout;
	vlayout->setSpacing(1);
	vlayout->setMargin(0);

	vlayout->addWidget(m_image);
	vlayout->addWidget(m_nameLabel);

	QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
	
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->setSpacing(10);
	
	uploadIndicator = new QLabel();
	uploadIndicator->hide();
	
	buttonsLayout->addItem(spacer);
	buttonsLayout->addWidget(m_edit);//client requested to remove
	buttonsLayout->addWidget(m_upload);//client requested to remove
	buttonsLayout->addWidget(m_delete);
	buttonsLayout->addWidget(uploadIndicator);
	buttonsLayout->addItem(new QSpacerItem(*spacer));

	vlayout->addLayout(buttonsLayout);

	setLayout(vlayout);

	vlayout->setAlignment(m_image, Qt::AlignHCenter);
	vlayout->setAlignment(m_nameLabel, Qt::AlignHCenter);

	m_edit->setIcon(QIcon(":/images/new/edit.png"));//client requested to remove
	m_upload->setIcon(QIcon(":/images/new/upload-blue.png"));//client requested to remove
	m_delete->setIcon(QIcon(":/images/new/delete.png"));
}


QPixmap VideoItemWidget::createThumbnail() {
	return QPixmap::fromImage( 
		VideoFile(filePath()).firstFrame().scaled(thumbnailSize(), 
			Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
}

void VideoItemWidget::setThumbnail() {
	QPixmap pixmap = createThumbnail();
	
	if (!pixmap.isNull()) {
		m_image->setIcon(QIcon());
		int imageWid = pixmap.size().width();
		int imageHei = pixmap.size().height();
		if (imageWid / imageHei > 100 / 150)
		{
			m_image->setIcon(pixmap.scaled(QSize(150, imageHei*150/imageWid)));
			m_image->setIconSize(QSize(150 , 100));
		}
		else
		{
			m_image->setIcon(pixmap.scaled(QSize(imageWid*100/imageHei, 100)));
			m_image->setIconSize(QSize(150, 100));
		}

		m_image->setContentsMargins(0, 0, 0, 0);
	}	else {
		m_image->setIconSize(thumbnailSize());
		m_image->setIcon(QIcon(":/images/new/mp4.png"));
	}	
}


void VideoItemWidget::showEvent(QShowEvent* event) {
	adjustCheckboxPosition();
}

void VideoItemWidget::adjustCheckboxPosition() {
	int leftOffset = (width() - m_image->width())/2;
	m_selectionCheckbox->move(leftOffset, m_image->height());
}


bool VideoItemWidget::isSelected() {
	return m_selectionCheckbox->isChecked();
}

void VideoItemWidget::setSelected(bool selected) {
	m_selectionCheckbox->setChecked(selected);
}

/*skyli added begin*/
void VideoItemWidget::onImageBtnClicked()
{
	showFilePath();
}
void VideoItemWidget::showFilePath()
{
	bool res = QDesktopServices::openUrl(QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)));
	if (!res)
	{
		QMessageBox msgBox;
		msgBox.setText("Can not open storage folder.");
		msgBox.exec();
	}
}
/*skyli added end*/
