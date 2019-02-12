#pragma once

#include <QWidget>
#include <QString>
#include <QPushButton>

class GrayButton : public QPushButton
{
	Q_OBJECT
public:
	GrayButton(const QString &text, QWidget *parent = nullptr);
};
