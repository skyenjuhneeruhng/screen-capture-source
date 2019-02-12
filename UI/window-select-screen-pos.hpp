#pragma once

#include <QDialog>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QScreen>
#include <memory>
#include "ui_SelectScreenPos.h"

class SelectScreenPosWindow : public QDialog {
	Q_OBJECT
public:
	SelectScreenPosWindow(QWidget *parent = nullptr);
	virtual ~SelectScreenPosWindow() override;

	QScreen* hostScreen() const;
	void setText(QString text) const;
	void setAcceptMouseInput(bool mouseInput);
	void setAcceptKeyboardInput(bool keyboardInput);

protected:
	virtual void keyPressEvent(QKeyEvent* event) override;
	virtual void showEvent(QShowEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;

private:
	void setupGeometry();

	Ui::SelectScreenPos* m_ui;
	bool m_isMoving;
	bool m_ignoreKeyboard;
	bool m_ignoreMouse;
	QPoint m_lastMousePos;
	QScreen* m_hostMonitor;
};
