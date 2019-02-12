#pragma once

#include "features/custom-widgets/virtual-classes/overlay.hpp"
#include <QTimer>
#include <QLabel>
#include <QString>

#define CLOCK_TICK 50

class	Timecode : public Overlay {
	Q_OBJECT
		
public:
	Timecode(QWidget *parent = nullptr);
	void setRestrictedArea(QRect);
	
	void start();
	void pause();
	void resume();
	void stop();
	
private slots:
	void ClockTick(); // milliseconds++
	
private:
	void showEvent(QShowEvent*) override;
	void manageVisibility();
	void setInitialTime();
	
	QPoint positionOnScreen();
	void createLabel();
	
	QString millisecondsAsTimecode();
	
	bool paused;
	
	unsigned int milliseconds;
	QTimer* timer;
	QLabel *label;
	
	QRect restrictedArea;
};