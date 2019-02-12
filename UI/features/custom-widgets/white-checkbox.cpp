#include "white-checkbox.hpp"

WhiteCheckbox::WhiteCheckbox(const QString &text, int maxWidthInSymbols, QWidget *parent) : QCheckBox(text, parent) {
	setCustomStyleSheet();
}

WhiteCheckbox::WhiteCheckbox(QWidget *parent) : QCheckBox(parent) {
	setCustomStyleSheet();
}


void WhiteCheckbox::setCustomStyleSheet() {
	setStyleSheet("QCheckBox { color: white; spacing: 6px; text-align: center; }"
	 						  "QCheckBox::indicator { border: 1px solid white; color: white; background: none; width: 12px; height: 12px; }"
								"QCheckBox::indicator:checked { image: url(:/images/new/checkbox-checked.png); }");
}