#pragma once

#include "deps/frameless-darkstyle/framelesswindow/framelesswindow.h"
#include "window-basic-main.hpp"
#include "video-item-widget.hpp"
#include "features/custom-widgets/white-checkbox.hpp"
#include "features/custom-widgets/gray-button.hpp"
#include <QShowEvent>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>



class VideoListWindow : public FramelessWindow
{
	Q_OBJECT
public:
	VideoListWindow(QWidget *parent = nullptr);
	virtual ~VideoListWindow() override;

protected:
	virtual void showEvent(QShowEvent* event)override;
	void closeEvent(QCloseEvent* event);

private slots:
	void SelectAllCheckboxToggled(bool);

	void VideoDeleted();
	void DeleteSelectedVideos();
	void UploadSelectedVideos();

	void EditClicked(QString filePath);//skyli added
public:
	void displayVideos();
private:
	void setupUI();
	QScrollArea* scrollArea(QWidget*);
	QHBoxLayout* toolbarLayout();
	void setupWindowProperties();


	void createListOfVideoItems();
	void writeListIntoGridLayout();
	void clearGridLayout();

	void removeVideoItemFromList(VideoItemWidget*);

	QList<VideoItemWidget*> selectedVideoItems();

  QString videosDirectory();
  QStringList videos();

	int m_lastVideoItemLoaded;
	QList<VideoItemWidget*> list;
	OBSBasic* m_main;
	QGridLayout* gridLayout;
	QScrollArea* m_scrollArea;

	QHBoxLayout* m_toolbarLayout;
	WhiteCheckbox *selectAll;
	GrayButton* deleteSelected;
	GrayButton* uploadSelected;

};
