#pragma once

#include <QWidget>
#include <QToolButton>
#include <QEvent>

class DisableableToolButton : public QToolButton
{
	Q_OBJECT
public:
	DisableableToolButton(QWidget *parent = nullptr);
private:
	void changeEvent(QEvent *event) override;
};
