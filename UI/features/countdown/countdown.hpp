#pragma once

#include "features/custom-widgets/virtual-classes/overlay.hpp"
#include <QDialog>
#include <QWindow>
#include <QTimer>
#include <QLabel>

class Countdown : public Overlay
{
	Q_OBJECT

public:
	Countdown(QWidget *parent = nullptr);
	
private slots:
	void CountOneDown();
	
private:
	QPoint positionOnScreen() override;
	void createLabel();

	int leftToCount;
	QTimer *timer;
	QLabel *label;
};
