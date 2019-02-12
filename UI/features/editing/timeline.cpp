#include "timeline.hpp"
#include <QPainter>
#include <QDebug>

Timeline::Timeline(qint64 duration, QWidget* parent) : QWidget(parent) {
	this->duration = (int)(duration);

	draggingLeftMarker = false;
	draggingRightMarker = false;
	
	setMinimumSize(10, 10); // to ensure rendering
	setMouseTracking(true); // for hover events	
}

void Timeline::showEvent(QShowEvent*) {
	cursor = place(0);
	leftMarker = place(0);
	rightMarker = place(duration);
}


int Timeline::beginningTime() {
	return time(leftMarker);
}

int Timeline::endTime() {
	return time(rightMarker);
}


int Timeline::timelineStart() {
	return MARKER_THICKNESS;
}

int Timeline::timelineWidth() {
	return width() - MARKER_THICKNESS * 2;
}


int Timeline::place(int milliseconds) {
	return timelineStart() + (milliseconds * timelineWidth() / duration);
}

int Timeline::time(int coordinate) {
	return (coordinate - timelineStart()) * duration / timelineWidth();
}


void Timeline::timelineClicked(QPoint pos) {
	if (isOutOfBounds(pos)) return;
	
	cursor = pos.x();
	emit CursorMovedTo(time(cursor));
	update();
}

void Timeline::leftMarkerDragged(QPoint pos) {
	if (isOutOfBounds(pos)) return;
	
	leftMarker = pos.x();
	emit LeftMarkerMovedTo(time(leftMarker));
	update();
}

void Timeline::rightMarkerDragged(QPoint pos) {
	if (isOutOfBounds(pos)) return;
	
	rightMarker = pos.x();
	emit RightMarkerMovedTo(time(rightMarker));
	update();
}


void Timeline::MoveCursorTo(qint64 milliseconds) {
	// qDebug() << "position" << milliseconds/1000;
	cursor = place((int)(milliseconds));
	update();
}


void Timeline::mouseMoveEvent(QMouseEvent* event) {
	QPoint pos = event->pos();
	
	bool dragging = draggingLeftMarker || draggingRightMarker;
	bool startingToDrag = !dragging && (event->buttons() == Qt::LeftButton);
	
	if (isOverLeftMarker(pos) || isOverRightMarker(pos))
		setCursor(QCursor(Qt::SizeHorCursor));
	else
		setCursor(QCursor());
	
	if (startingToDrag) {
		if (isOverLeftMarker(pos)) { draggingLeftMarker = true; }
		if (isOverRightMarker(pos)) { draggingRightMarker = true; }
	}
	
	if (dragging) {
		if (draggingLeftMarker) leftMarkerDragged(pos);
		if (draggingRightMarker) rightMarkerDragged(pos);
	}
}

void Timeline::mouseReleaseEvent(QMouseEvent* event) {
	bool dragging = draggingLeftMarker || draggingRightMarker;
	
	if (dragging) {
		if (draggingLeftMarker) { draggingLeftMarker = false; }
		if (draggingRightMarker) { draggingRightMarker = false; }		
	} else {
		timelineClicked(event->pos());
	}
}


bool Timeline::isOverLeftMarker(QPoint pos) {
	return (pos.x() >= (leftMarker-MARKER_THICKNESS)) && (pos.x() <= (leftMarker+MARKER_THICKNESS));
}

bool Timeline::isOverRightMarker(QPoint pos) {
	return pos.x() >= (rightMarker-MARKER_THICKNESS) && pos.x() <= (rightMarker+MARKER_THICKNESS);
}

bool Timeline::isOutOfBounds(QPoint pos) {
	return pos.x() < place(0) || pos.x() > place(duration);
}


void Timeline::setColor(QPainter &painter, QString colorName) {
	painter.setBrush(QBrush(QColor(colorName)));
}

void Timeline::drawHandle(QPainter &painter, int marker) {
	painter.drawLine(marker-2, 12, marker-2, 37);
	painter.drawLine(marker+2, 12, marker+2, 37);
}


void Timeline::paintEvent(QPaintEvent* event) {
	QPainter painter(this);
	
	
	QRect timelineRect = QRect(
		timelineStart(), TOP_THICKNESS, 
		timelineWidth(), height() - TOP_THICKNESS*2);

	QRect outerRect = QRect(
		leftMarker - MARKER_THICKNESS, 0,
		(rightMarker - leftMarker) + MARKER_THICKNESS*2, height());

	QRect holeRect = QRect(
		leftMarker + MARKER_THICKNESS, TOP_THICKNESS,
		(rightMarker - leftMarker) - MARKER_THICKNESS*2, height() - TOP_THICKNESS*2);

	QRect cursorRect = QRect(
		cursor - CURSOR_THICKNESS, 0,
		CURSOR_THICKNESS*2, height());
		
	
	painter.setPen(Qt::NoPen);
	setColor(painter, TIMELINE_COLOR);
	painter.drawRoundedRect(timelineRect, BORDER_RADIUS, BORDER_RADIUS);
	
	painter.setPen(Qt::SolidLine);
	setColor(painter, MARKER_COLOR);
	painter.drawRoundedRect(outerRect, BORDER_RADIUS, BORDER_RADIUS);
	setColor(painter, TIMELINE_COLOR);
	painter.drawRoundedRect(holeRect, BORDER_RADIUS, BORDER_RADIUS);
	
	drawHandle(painter, leftMarker);
	drawHandle(painter, rightMarker);
		
	painter.fillRect(cursorRect, QColor(CURSOR_COLOR));
}