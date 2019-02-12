#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QPaintEvent>

#define TOP_THICKNESS 5
#define MARKER_THICKNESS 10
#define CURSOR_THICKNESS 2

#define CURSOR_COLOR "red"
#define MARKER_COLOR "yellow"
#define TIMELINE_COLOR "#C0C1C2"
#define HANDLE_TALLNESS 0.3
#define BORDER_RADIUS 5

class Timeline : public QWidget
{
	Q_OBJECT

public:
	Timeline(qint64 duration, QWidget* parent = nullptr);
	
	int beginningTime();
	int endTime();

public slots:
	void MoveCursorTo(qint64);
	
signals:
	void LeftMarkerMovedTo(int);
	void RightMarkerMovedTo(int);
	void CursorMovedTo(int);

protected:
	void mouseMoveEvent(QMouseEvent*) override;
	void mouseReleaseEvent(QMouseEvent*) override;
	void showEvent(QShowEvent*) override;
	void paintEvent(QPaintEvent*) override;
	
private:
	int duration;

	int cursor;
	int leftMarker;
	int rightMarker;

	bool draggingLeftMarker;
	bool draggingRightMarker;

	void timelineClicked(QPoint);
	// void cursorDragged(QPoint);
	void leftMarkerDragged(QPoint);
	void rightMarkerDragged(QPoint);
	
	int time(int);
	int place(int);
	
	int timelineStart();
	int timelineWidth();

	bool isOverLeftMarker(QPoint);
	bool isOverRightMarker(QPoint);
	bool isOutOfBounds(QPoint pos);
	
	void setColor(QPainter&, QString);
	void drawHandle(QPainter&, int);
};
