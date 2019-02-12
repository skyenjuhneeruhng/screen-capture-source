#include "gray-button.hpp"

GrayButton::GrayButton(const QString &text, QWidget *parent) : QPushButton(text, parent) {
	setStyleSheet("QPushButton { background-color: #C0C1C2; color: black; padding: 5px 10px; border: none; }"
								"QPushButton:pressed { background-color: #97999b; }");
}