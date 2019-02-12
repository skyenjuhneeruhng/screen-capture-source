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

#include <QGraphicsDropShadowEffect>
#include "framelesswindow.h"

FramelessWindow::FramelessWindow(QWidget *parent): QDialog(parent)
{
  setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
  // append minimize button flag in case of windows,
  // for correct windows native handling of minimize function
#if defined(Q_OS_WIN)
  setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);
#endif
  setAttribute(Qt::WA_TranslucentBackground);

  setupUi(this);

  restoreButton->setVisible(false);

  //shadow under window title text
  //QGraphicsDropShadowEffect *textShadow = new QGraphicsDropShadowEffect;
  //textShadow->setBlurRadius(4.0);
  //textShadow->setColor(QColor(0,0,0));
  //textShadow->setOffset(0.0);
  //titleText->setGraphicsEffect(textShadow);

  ////window shadow
  //QGraphicsDropShadowEffect *windowShadow = new QGraphicsDropShadowEffect;
  //windowShadow->setBlurRadius(9.0);
  //windowShadow->setColor(palette().color(QPalette::Highlight));
  //windowShadow->setOffset(0.0);
  //windowFrame->setGraphicsEffect(windowShadow);

  maximizeButton->setVisible(false);
  minimizeButton->setVisible(false);
  restoreButton->setVisible(false);

  styleWindow(true, true);

  QObject::connect(qApp, &QGuiApplication::applicationStateChanged, this, &FramelessWindow::on_applicationStateChanged);
}

void FramelessWindow::setMinimizeButtonVisible(bool visible) {
	#ifdef _WIN32
	  minimizeButton->setVisible(visible);
	#endif
}

void FramelessWindow::changeEvent(QEvent *event)
{
	emit stateChanged(event);
  if (event->type() == QEvent::WindowStateChange) {
    if (windowState().testFlag(Qt::WindowNoState)) {
      on_restoreButton_clicked();
      event->ignore();
    } else if (windowState().testFlag(Qt::WindowMaximized)) {
      on_maximizeButton_clicked();
      event->ignore();
    }
  }
  event->accept();
}


void FramelessWindow::setContent(QWidget *w)
{
  QVBoxLayout* layout = new QVBoxLayout;
  layout->setMargin(0);
  layout->addWidget(w);
  windowContent->setLayout(layout);
}

void FramelessWindow::setLayoutInsteadOfContent(QLayout *layout)
{
  layout->setMargin(5);
  windowContent->setLayout(layout);
}


QWidget * FramelessWindow::windowWidget()
{
	return windowFrame;
}

void FramelessWindow::setWindowTitle(const QString &text)
{
	titleText->setText(text);
}

void FramelessWindow::styleWindow(bool bActive, bool bNoState)
{
	if (QFile::exists("style.qss")) {
		QFile file("style.qss");
		if (file.open(QFile::ReadOnly))
		{
			QString style = file.readAll();
			setStyleSheet(style);
		}
	}
	else
	{
		QFile file(":/style/style.qss");
		if (file.open(QFile::ReadOnly))
		{
			QString style = file.readAll();
			setStyleSheet(style);
		}
	}
}

void FramelessWindow::on_applicationStateChanged(Qt::ApplicationState state)
{
  if (windowState().testFlag(Qt::WindowNoState)) {
    if (state == Qt::ApplicationActive) {
      styleWindow(true, true);
    } else {
      styleWindow(false, true);
    }
  } else if (windowState().testFlag(Qt::WindowMaximized)) {
    if (state == Qt::ApplicationActive) {
      styleWindow(true, false);
    } else {
      styleWindow(false, false);
    }
  }
}

void FramelessWindow::on_minimizeButton_clicked()
{
  setWindowState(Qt::WindowMinimized);
}

void FramelessWindow::on_restoreButton_clicked() {
  //restoreButton->setVisible(false);
  //maximizeButton->setVisible(true);
  setWindowState(Qt::WindowNoState);
  styleWindow(true, true);
}
void FramelessWindow::on_maximizeButton_clicked()
{
  //restoreButton->setVisible(true);
  //maximizeButton->setVisible(false);
  setWindowState(Qt::WindowMaximized);
  styleWindow(true, false);
}
void FramelessWindow::on_closeButton_clicked()
{
  close();//skyli changed
}

void FramelessWindow::on_windowTitlebar_doubleClicked()
{
  if (windowState().testFlag(Qt::WindowNoState)) {
    on_maximizeButton_clicked();
  } else if (windowState().testFlag(Qt::WindowMaximized)) {
    on_restoreButton_clicked();
  }
}
void FramelessWindow::closeEvent(QCloseEvent *event)
{
	emit aboutToClose(event);
}

