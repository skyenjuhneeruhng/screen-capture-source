#include "overlay.hpp"
#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

Overlay::Overlay(QWidget *parent) : QDialog(parent) {
	// transparentBackground
	setWindowFlags(Qt::Widget | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::NoDropShadowWindowHint | Qt::SubWindow);
	setParent(0); // Top-Level Widget
	setAttribute(Qt::WA_TranslucentBackground, true);
	// setAttribute(Qt::WA_NoSystemBackground, true);
	setBackgroundRole(QPalette::NoRole);	
}

void Overlay::showEvent(QShowEvent *event) {
	move(positionOnScreen());
}