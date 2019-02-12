#include "disableable-tool-button.hpp"

#include <QGraphicsOpacityEffect>

DisableableToolButton::DisableableToolButton(QWidget *parent) : QToolButton(parent) {
	
}

void DisableableToolButton::changeEvent(QEvent *event) {
	QToolButton::changeEvent(event);
	
	#ifdef __APPLE__
		return;
	#endif
	
	#ifdef _WIN32
		if (event->type() != QEvent::EnabledChange) { return; }
	
		if (isEnabled()) {
			setGraphicsEffect(NULL);
		} else {
			QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect;
			opacityEffect->setOpacity(0.5);
			setGraphicsEffect(opacityEffect);
		}
	#endif
}