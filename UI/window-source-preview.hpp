#pragma once

#include "window-select-screen-pos.hpp"
#include "qt-display.hpp"

class SourcePreviewWindow : public SelectScreenPosWindow {
	Q_OBJECT
	friend void DrawPreview(void *data, uint32_t cx, uint32_t cy);
public:
	SourcePreviewWindow(obs_sceneitem_t* item, QWidget *parent = nullptr);
	virtual ~SourcePreviewWindow() override;
	void setWindowBounds(QRect bounds);

	static void DrawPreview(void *data, uint32_t cx, uint32_t cy);

protected:
	void moveEvent(QMoveEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

private slots:
	void VideoRenderCallback();
	void ResizeToBounds();

signals:
	void VideoRendered();
	void SourceLoaded();

private:
	obs_sceneitem_t* m_sceneitem;
	OBSQTDisplay* m_display;
	bool m_isSizing;
	bool m_shouldTriggerResizeEvent;
	bool m_shouldTriggerMoveEvent;
	QRect m_windowBounds;
	
	bool shouldResizeToBounds = true;
};
