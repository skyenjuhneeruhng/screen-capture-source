/*
###############################################################################
#                                                                             #
# The MIT License                                                             #
#                                                                             #
# Copyright (C) 2017 by Juergen Skrotzky (JorgenVikingGod@gmail.com)          #
#               >> https://github.com/Jorgen-VikingGod                        #
#                                                                             #
# Sources: https://github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle  #
#                                                                             #
###############################################################################
*/

#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QDialog>
#include "ui_framelesswindow.h"

class FramelessWindow: public QDialog, private Ui::FramelessWindow
{
  Q_OBJECT

public:
  explicit FramelessWindow(QWidget *parent = 0);
	void setMinimizeButtonVisible(bool);
  void setContent(QWidget *w);
  void setLayoutInsteadOfContent(QLayout *layout);
  QWidget* windowWidget();

private:
  void styleWindow(bool bActive, bool bNoState);

signals:
	void stateChanged(QEvent* evt);
	//skyli added
	void aboutToClose(QCloseEvent *ev);//skyli added
public slots:
  void setWindowTitle(const QString &text);

private slots:
  void on_applicationStateChanged(Qt::ApplicationState state);
  void on_minimizeButton_clicked();
  void on_restoreButton_clicked();
  void on_maximizeButton_clicked();
  void on_closeButton_clicked();
  void on_windowTitlebar_doubleClicked();

protected:
  virtual void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);

protected:
  QHBoxLayout contentLayout;
};

#endif // FRAMELESSWINDOW_H
