#include "countdown.hpp"
#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsEffect>
#include <QDebug>

Countdown::Countdown(QWidget *parent) : Overlay(parent) {
	createLabel();
	
	leftToCount = 5;

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(CountOneDown()));
	
	timer->start(1000);
	CountOneDown();
}

void Countdown::CountOneDown() {
	if (leftToCount == 0) {
		close();
		qApp->processEvents();
	} else {
		label->setText(QString::number(leftToCount));
		leftToCount--;
	}
}


QPoint Countdown::positionOnScreen() {
	QRect screenGeometry = qApp->desktop()->screenGeometry(this);

	int x = (screenGeometry.width() - width()) / 2;
	int y = (screenGeometry.height() - height()) / 2;
	return QPoint(x, y);
}

void Countdown::createLabel() {
	label = new QLabel(this);
	label->setStyleSheet("font-size: 300px; color: white;");
	QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
	label->setGraphicsEffect(shadow);
	
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(label);
	setLayout(layout);
}
