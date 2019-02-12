#include "video-list-window.hpp"
#include "confirm-deletion.hpp"
#include "features/editing/editing-window.hpp"
#include <QFile>
#include <QDir>
#include <QScrollBar>
#include <QtAV>
#include <QtAVWidgets>
#include <QMessageBox>

VideoListWindow::VideoListWindow(QWidget* parent) :
	FramelessWindow(parent),
	m_lastVideoItemLoaded(0),
	m_main(qobject_cast<OBSBasic*>(App()->GetMainWindow()))
{
		setupUI();
		setupWindowProperties();
		//skyli added
		QtAV::Widgets::registerRenderers();
}

VideoListWindow::~VideoListWindow()
{
	for(int i = 0; i < list.length(); ++i)
	{
		delete list[i];
	}
}


void VideoListWindow::displayVideos() 
{
	clearGridLayout();
	list.clear();
	createListOfVideoItems();
	writeListIntoGridLayout();
}

void VideoListWindow::VideoDeleted()
{
	VideoItemWidget *videoItem = qobject_cast<VideoItemWidget*>(sender());

	clearGridLayout();	
	removeVideoItemFromList(videoItem);
	writeListIntoGridLayout();
}

void VideoListWindow::DeleteSelectedVideos()
{
	//client requested to remove
	
	if (!confirmDeletion()) return;

	for (VideoItemWidget* videoItem : selectedVideoItems()) {
		videoItem->eraseFile();
		removeVideoItemFromList(videoItem);
	}
	
	clearGridLayout();	
	writeListIntoGridLayout();
	
}

void VideoListWindow::UploadSelectedVideos()
{
	
	for (VideoItemWidget* videoItem : selectedVideoItems()) {
		videoItem->uploadFile();
	}
}


QList<VideoItemWidget*> VideoListWindow::selectedVideoItems() {
	QList<VideoItemWidget*> selected;

	for (VideoItemWidget* videoItem : list) {
		if (videoItem->isSelected()) { selected.push_back(videoItem); }
	}

	return selected;
}


QString VideoListWindow::videosDirectory() {
	return config_get_string(m_main->Config(), "SimpleOutput", "FilePath"); // ~/Movies
}

QStringList VideoListWindow::videos() {
	QStringList relativePaths = QDir(videosDirectory()).entryList(QStringList() << "*.mp4", QDir::Files);
	QStringList absolutePaths = QStringList();
	
	for (QString relativePath : relativePaths)
		absolutePaths << QDir(videosDirectory()).absoluteFilePath(relativePath);
	
	return absolutePaths;
}


void VideoListWindow::createListOfVideoItems() {
	for (QString videoPath : videos()) {
		VideoItemWidget *videoItem = new VideoItemWidget(videoPath);
		list.push_back(videoItem);
		qDebug() << "push_back" << videoPath;
		
		connect(videoItem, SIGNAL(FileErased()), this, SLOT(VideoDeleted()));
		connect(videoItem, SIGNAL(EditClicked(QString)), this, SLOT(EditClicked(QString)));//skyli added
		// connect(videoItem, SIGNAL(EditClosed()), this, SLOT(EditClosed()));//skyli added
	}
}

void VideoListWindow::EditClicked(QString filePath)
{
	EditingWindow *editingWindow = new EditingWindow(filePath);//skyli added
	editingWindow->setModal(true);
	editingWindow->setWindowFlags(Qt::FramelessWindowHint);
	editingWindow->setAttribute(Qt::WA_DeleteOnClose);//skyli added
	editingWindow->show();//skyli added
}


void VideoListWindow::removeVideoItemFromList(VideoItemWidget* videoItem) {
	list.removeOne(videoItem);
	delete videoItem;
}


void VideoListWindow::writeListIntoGridLayout() {
	for (int i = 0; i < list.length(); i++) {
		VideoItemWidget* videoItem = list[i];
		gridLayout->addWidget(videoItem, i / 3, i % 3);
	}

	gridLayout->setAlignment(Qt::AlignTop);
}

void VideoListWindow::clearGridLayout() {
	for (int i = 0; i < gridLayout->count(); i++)
		gridLayout->removeItem(gridLayout->itemAt(i));
}


void VideoListWindow::setupUI() {
	resize(750, 750);
	gridLayout = new QGridLayout;

	displayVideos();
	
	QWidget* content = new QWidget(this);
	content->setLayout(gridLayout);

	QVBoxLayout* verticalLayout = new QVBoxLayout;
	verticalLayout->setSpacing(0);

	verticalLayout->addLayout(toolbarLayout());
	verticalLayout->addWidget(scrollArea(content));

	setLayoutInsteadOfContent(verticalLayout);
}


QHBoxLayout* VideoListWindow::toolbarLayout() {
	selectAll = new WhiteCheckbox("Select All");
	deleteSelected = new GrayButton("Delete Selected");//client requested to remove
	uploadSelected = new GrayButton("Upload Selected");//client requested to remove

	QSpacerItem* leftsideSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QSpacerItem* betweenButtons = new QSpacerItem(10, 0);

	m_toolbarLayout = new QHBoxLayout;
	m_toolbarLayout->setSpacing(10);
	m_toolbarLayout->setMargin(5);

	m_toolbarLayout->addItem(leftsideSpacer);
	m_toolbarLayout->addWidget(selectAll);
	m_toolbarLayout->addItem(betweenButtons);
	m_toolbarLayout->addWidget(deleteSelected);//client requested to remove
	m_toolbarLayout->addWidget(uploadSelected);//client requested to remove

	connect(selectAll, SIGNAL(toggled(bool)), this, SLOT(SelectAllCheckboxToggled(bool)));
	connect(deleteSelected, SIGNAL(clicked()), this, SLOT(DeleteSelectedVideos()));//client requested to remove
	connect(uploadSelected, SIGNAL(clicked()), this, SLOT(UploadSelectedVideos()));//client requested to remove

	return m_toolbarLayout;
}

void VideoListWindow::SelectAllCheckboxToggled(bool checked) {
	for (VideoItemWidget* videoItem : list)
		videoItem->setSelected(checked);
}


QScrollArea* VideoListWindow::scrollArea(QWidget* content) {
	m_scrollArea = new QScrollArea;
	m_scrollArea->setWidget(content);
	m_scrollArea->setWidgetResizable(true);
	m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	m_scrollArea->setWidgetResizable(false);

	m_scrollArea->setObjectName("videoScrollArea");
	m_scrollArea->setStyleSheet("background-color: #253242; border: none;");

	return m_scrollArea;
}


void VideoListWindow::setupWindowProperties() {

	setAttribute(Qt::WA_DeleteOnClose, true);
	setWindowTitle("Video List");
	

	//setMinimumSize(700, 550);
	//setMaximumSize(700, 550);
}

void VideoListWindow::showEvent(QShowEvent* event)
{
	Q_UNUSED(event);
	int w = width() - m_scrollArea->verticalScrollBar()->width() - 30;
	m_scrollArea->widget()->setMinimumWidth(w);
}

void VideoListWindow::closeEvent(QCloseEvent *event)
{
	m_main->videoListWindowOpened = false;
}