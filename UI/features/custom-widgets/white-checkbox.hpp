#pragma once

#include <QWidget>
#include <QString>
#include <QCheckBox>

class WhiteCheckbox : public QCheckBox
{
	Q_OBJECT
public:
	WhiteCheckbox(const QString &text = nullptr, int maxWidthInSymbols = 0, QWidget *parent = nullptr);
	WhiteCheckbox(QWidget *parent);

	void setCustomStyleSheet();
};
