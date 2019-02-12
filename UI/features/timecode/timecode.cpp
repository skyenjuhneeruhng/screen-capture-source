#include "timecode.hpp"
#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QChar>
#include <QDebug>

Timecode::Timecode(QWidget *parent) : Overlay(parent) {
	createLabel();
	label->setText(millisecondsAsTimecode());
	
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(ClockTick()));
}


void Timecode::start() {
	milliseconds = 0;
	timer->start(CLOCK_TICK);
	manageVisibility();
}

void Timecode::pause() {
	timer->stop();
}

void Timecode::resume() {
	timer->start(CLOCK_TICK);
	manageVisibility();
}

void Timecode::stop() {
	timer->stop();
	setInitialTime();
	//hide();
}

void Timecode::setInitialTime()
{
	milliseconds = 0;
	label->setText(millisecondsAsTimecode());
}
void Timecode::manageVisibility() {
	//skyli changed
	// if (restrictedArea.intersects(QRect(positionOnScreen(), QSize(100, 20)))) {
	// 	hide();
	// } else {
		show();
	// }
}


void Timecode::ClockTick() {
	milliseconds += CLOCK_TICK;
	label->setText(millisecondsAsTimecode());
}

QString Timecode::millisecondsAsTimecode() {
	int seconds = (milliseconds / 1000) % 60;
	int minutes = ((milliseconds / 1000) / 60) % 60;
	int hours = ((milliseconds / 1000) / 60) / 60;
	int frames = (milliseconds % 1000) / 33;
	return QString("Recording Elapse: %1:%2:%3.%4")
		.arg(QString::number(hours), 2, QChar('0'))
		.arg(QString::number(minutes), 2, QChar('0'))
		.arg(QString::number(seconds), 2, QChar('0'))
		.arg(QString::number(frames), 2, QChar('0'));
}


QPoint Timecode::positionOnScreen() {
	//QRect screenGeometry = qApp->desktop()->screenGeometry(this);

	//int x = screenGeometry.width() - width();
	int x = restrictedArea.x() + restrictedArea.width() - width();//skyli changed
	int y = restrictedArea.y();
	return QPoint(x, y);
}

void Timecode::createLabel() {
	label = new QLabel(this);
	label->setStyleSheet("color: white; "
		"text-align:center;"
    	 "font-size: 11px; font-family: monospace;"
		 "padding-left: 10px;");
	
	QHBoxLayout* layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(label);
	setLayout(layout);
	milliseconds = 0;
	label->setText(millisecondsAsTimecode());
}


void Timecode::setRestrictedArea(QRect screenRegion) {
	restrictedArea = screenRegion;
}

void Timecode::showEvent(QShowEvent *event) {
	//move(positionOnScreen());//skyli changed
	manageVisibility();
}