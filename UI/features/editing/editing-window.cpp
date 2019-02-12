#include "editing-window.hpp"
#include "features/custom-widgets/video-file.hpp"
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QTimer>
#include <QMessageBox>
#include <QPixmap>

EditingWindow::EditingWindow(QString filePath, QWidget* parent) : FramelessWindow(parent) {
	this->filePath = filePath;
	
  setupWindowProperties();
  setupUI();
	
	connect(saveButton, SIGNAL(clicked()), this, SLOT(Commit()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(Cancel()));
}

EditingWindow::~EditingWindow() {
	delete timeIndicator;
	delete timeline;
	delete renderer;
	delete player;
}

void EditingWindow::closeFile() {
	if (player->state() == QtAV::AVPlayer::StoppedState) return;

	QEventLoop waitFotStop;
	connect(player, SIGNAL(stopped()), &waitFotStop, SLOT(quit()));
	
	player->stop();
	waitFotStop.exec();
}


void EditingWindow::setupUI() {
	QSpacerItem* leftsideSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
	saveButton = new GrayButton("Save");
	cancelButton = new GrayButton("Cancel");
	
	playStopButton = new QToolButton;
	setPlaying(false);
	
	setupPlayer();
	setupTimeline();
	
	QHBoxLayout* timelineLayout = new QHBoxLayout;
	timelineLayout->setMargin(10);
	timelineLayout->addWidget(playStopButton);
	timelineLayout->addWidget(timeline);
	
	QWidget* timelineLayoutWrapper = new QWidget;
	timelineLayoutWrapper->setStyleSheet("background-color: #111A23");
	timelineLayoutWrapper->setLayout(timelineLayout);
	
	QHBoxLayout* toolbarLayout = new QHBoxLayout;
	toolbarLayout->setMargin(5);
	toolbarLayout->setSpacing(10);
	toolbarLayout->addItem(leftsideSpacer);
	toolbarLayout->addWidget(saveButton);
	toolbarLayout->addWidget(cancelButton);

	timeIndicator = new QLabel(this);
	timeIndicator->setText(timecode(0) + "/" + timecode(player->duration()));

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setSpacing(0);
	layout->addLayout(toolbarLayout);
	layout->addWidget(renderer);
	layout->addWidget(timelineLayoutWrapper);
	layout->addWidget(timeIndicator);
	
	setLayoutInsteadOfContent(layout);
}

void EditingWindow::setupWindowProperties() {
	setAttribute(Qt::WA_TranslucentBackground, false);
	setAttribute(Qt::WA_DeleteOnClose, true);
	setWindowTitle("Editing");
	resize(600, 500);
}

void EditingWindow::setPlaying(bool isPlaying) {
	disconnect(playStopButton, SIGNAL(clicked()), NULL, NULL);
	if (isPlaying) {
		playStopButton->setIcon(QIcon(":/images/new/small-stop.png"));
		connect(playStopButton, SIGNAL(clicked()), this, SLOT(Stop()));
	} else {
		playStopButton->setIcon(QIcon(":/images/new/small-play.png"));
		connect(playStopButton, SIGNAL(clicked()), this, SLOT(Play()));
	}
}


void EditingWindow::setupTimeline() {
	timeline = new Timeline(player->duration());
	timeline->setMinimumHeight(50);
	timeline->setMaximumWidth(width() - 10);
	
	displayFirstFrame();
	
	connect(player, SIGNAL(positionChanged(qint64)), timeline, SLOT(MoveCursorTo(qint64)));

	connect(timeline, SIGNAL(CursorMovedTo(int)), this, SLOT(SeekTo(int)));
	connect(timeline, SIGNAL(LeftMarkerMovedTo(int)), this, SLOT(SetBeginning(int)));
	connect(timeline, SIGNAL(RightMarkerMovedTo(int)), this, SLOT(SetEnd(int)));
}

void EditingWindow::displayFirstFrame() {
	//skyli changed
	player->play();
	/*
	QPixmap firstFrame = QPixmap::fromImage( 
		VideoFile(filePath).firstFrame().scaled(renderer->geometry().size(), 
			Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
	QPalette palette;
	palette.setBrush(renderer->backgroundRole(), QBrush(firstFrame));
	renderer->setPalette(palette);
	*/
	//renderer->set

	// pauseAfterPlays = true;
}

void EditingWindow::setupPlayer() {
	//QtAV::Widgets::registerRenderers();//skyli changed
	
	renderer = new QtAV::GLWidgetRenderer2;
	renderer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	
	player = new QtAV::AVPlayer;
	player->setRenderer(renderer);
	player->setFile(filePath);
	player->setRelativeTimeMode(false);
	player->setMediaEndAction(QtAV::MediaEndAction_KeepDisplay);
	player->setAsyncLoad(false);
	player->setSeekType(QtAV::AccurateSeek);
	
	bool success = player->load();
	audioBackends = player->audio()->backends();//skyli
	curPosition = player->startPosition();//skyli

	if (!success) { qDebug() << "Couldn't load" << filePath; return; }
	
	qDebug() << "duration" << player->duration()/1000;

	connect(player, SIGNAL(stateChanged(QtAV::AVPlayer::State)), this, SLOT(PlayerStateChanged(QtAV::AVPlayer::State)));
	connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(PositionChanged(qint64)));
	//skyli changed
	connect(player, SIGNAL(startPositionChanged(qint64)), this, SLOT(StartPositionChanged(qint64)));
	connect(player, SIGNAL(stopPositionChanged(qint64)), this, SLOT(StopPositionChanged(qint64)));
}
//skyli added
void EditingWindow::StopPositionChanged(qint64 stopPos)
{
	qint64 pos = player->position();
	if (stopPos < pos && player->state() == QtAV::AVPlayer::PlayingState)
	{
		Stop();
	}
}
//skyli added
void EditingWindow::StartPositionChanged(qint64 startPos)
{
	qint64 pos = player->position();
	if (startPos >= pos && player->state() == QtAV::AVPlayer::PlayingState)
	{
		SeekTo(startPos);
		
	}
}

void EditingWindow::SeekTo(int position) {
	curPosition = (qint64)position;//skyli
	player->setPosition((qint64)(position));
	timeIndicator->setText(timecode(position) + "/" + timecode(player->duration()));
}

void EditingWindow::SetBeginning(int position) {
	// qDebug() << "start" << position/1000;
	player->setStartPosition((qint64)(position));
	// SeekTo(position);
}

void EditingWindow::SetEnd(int position) {
	// qDebug() << "end" << position/1000;
	player->setStopPosition((qint64)(position));
	// SeekTo(position);
}


void EditingWindow::Play() {
	// player->setPosition((qint64)(timeline->beginningTime()));
	if (player->state() == QtAV::AVPlayer::StoppedState) {
		player->audio()->setBackends(audioBackends);
		player->play();
		SeekTo(curPosition);
		
	} else {
		//player->stop();
		//playAfterStops = true;
	}

}

void EditingWindow::PositionChanged(qint64 position) {
	//if (pauseAfterPlays) {
	//	player->pause();
	//	pauseAfterPlays = false;
	//}
	timeIndicator->setText(timecode(position) + "/" + timecode(player->duration()));
}

void EditingWindow::PlayerStateChanged(QtAV::AVPlayer::State state) {
	if (state == QtAV::AVPlayer::PlayingState) {
		setPlaying(true);

		
		// if (pauseAfterPlays) {
		// 	Stop();
		// 	pauseAfterPlays = false;
		// }
	} else {
		setPlaying(false);
		player->audio()->setBackends(QStringList() << "null");//skyli
		curPosition = player->startPosition();//skyli
		/*
		if (playAfterStops) {
			player->play();
			playAfterStops = false;
		}*///skyli
	}
}

void EditingWindow::Stop() {
	
	
	player->stop();
	timeline->MoveCursorTo(player->startPosition());//skyli
}

QString timecode(int milliseconds) {
	int seconds = (milliseconds / 1000) % 60;
	int minutes = ((milliseconds / 1000) / 60) % 60;
	int hours = ((milliseconds / 1000) / 60) / 60;
	int frames = (milliseconds % 1000) / 33;
	return QString("%1:%2:%3")//skyli changed "%1:%2:%3.%4"
		.arg(QString::number(hours), 2, QChar('0'))
		.arg(QString::number(minutes), 2, QChar('0'))
		.arg(QString::number(seconds), 2, QChar('0'));//skyli changed
		//.arg(QString::number(frames), 1, QChar('0'));
}
void EditingWindow::Commit() {
	/*skyli changed*/
	QMessageBox msgBox;
	msgBox.setWindowTitle("Screen Recorder");
	msgBox.setText("Would like to save?");
	msgBox.setStandardButtons(QMessageBox::Yes);
	msgBox.addButton(QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);
	if(msgBox.exec() == QMessageBox::Yes){
	  	closeFile();
		videoTrimmer = new VideoTrimmer(filePath, timeline->beginningTime(), timeline->endTime());
		connect(videoTrimmer, SIGNAL(finished()), this, SLOT(TrimmingFinished()));
		videoTrimmer->start();
		
		saveButton->setText("Saving...");
	}else {
	  	//Cancel();
	}
	/*skyli changed*/
}

void EditingWindow::TrimmingFinished() {
	videoTrimmer->deleteLater();
	saveButton->setText("Save");
	close();
}


void EditingWindow::Cancel() {
	close();
}
//skyli added
void EditingWindow::closeEvent(QCloseEvent *ev){
	if (saveButton->text() == QString("Saving..."))
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle("Screen Recorder");
		msgBox.setText("Do you really want to discard changes to video edit? The video could be damaged.");
		msgBox.setStandardButtons(QMessageBox::Yes);
		msgBox.addButton(QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::No);
		if (msgBox.exec() == QMessageBox::Yes) {
			closeFile();
			ev->accept();
		}
		else {
			ev->ignore();
		}
	}
	else
	{
		closeFile();
	}
}