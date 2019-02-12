#pragma once

#include <QDialog>
#include <QWindow>
#include <QTimer>
#include <QLabel>

class Overlay : public QDialog
{
	Q_OBJECT

public:
	Overlay(QWidget *parent = nullptr);
	
private:
	void showEvent(QShowEvent *event);
	virtual QPoint positionOnScreen() = 0;
};
