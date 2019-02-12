#pragma once

#include "deps/frameless-darkstyle/framelesswindow/framelesswindow.h"
#include "features/custom-widgets/gray-button.hpp"
#include "custom-widgets/video-trimmer.hpp"
#include "timeline.hpp"
#include <QToolButton>
#include <QtAV>
#include <QtAVWidgets>

class EditingWindow : public FramelessWindow
{
	Q_OBJECT

public:
	EditingWindow(QString filePath, QWidget *parent = nullptr);
	virtual ~EditingWindow() override;

protected:
	void closeEvent(QCloseEvent *ev);//skyli added

public slots:
	void SeekTo(int);
	void SetBeginning(int);
	void SetEnd(int);
	
	void Commit();
	void Cancel();

	
private slots:
	void Play();
	void Stop();
	
	void PlayerStateChanged(QtAV::AVPlayer::State);
	void PositionChanged(qint64);

	//skyli added
	void StartPositionChanged(qint64);
	void StopPositionChanged(qint64);

	void TrimmingFinished();
	
private:
	GrayButton* saveButton;
	GrayButton* cancelButton;
	QToolButton* playStopButton;
	Timeline* timeline;

	QStringList audioBackends;
	qint64 curPosition;

	QLabel* timeIndicator;
	
	QString filePath;
	
	QtAV::GLWidgetRenderer2* renderer;
	QtAV::AVPlayer* player;
	
	bool playAfterStops = false;
	bool pauseAfterPlays = false;

	VideoTrimmer* videoTrimmer;
	
  void setupUI();
  void setupWindowProperties();
	void setupPlayer();
	void setupTimeline();
	void displayFirstFrame();
	
	void setPlaying(bool);
	
	void closeFile();

};
QString timecode(int milliseconds);
