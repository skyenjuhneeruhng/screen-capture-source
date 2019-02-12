#pragma once

#include "features/custom-widgets/white-checkbox.hpp"
#include <QWidget>
#include <QToolButton>
#include <QImage>
#include <QLabel>
#include <QHBoxLayout>
#include <QNetworkReply>


class VideoItemWidget : public QWidget
{
	Q_OBJECT
public:
	VideoItemWidget(QString path, QWidget* parent = nullptr);
	virtual ~VideoItemWidget() override;

	void SetPath(QString path);
	
	void eraseFile();
	void uploadFile();

	bool isSelected();
	void setSelected(bool);

private slots:
	void EditButtonClicked();
	void UploadButtonClicked();
	void DeleteButtonClicked();

	void UploadFinished(QNetworkReply*);

	//skyli added
	void onImageBtnClicked();

signals:
	void FileErased();
	void EditClicked(QString filePath);//skyli added

private:
	void SetupUI();

	QString filePath();
	QString name();
	QSize thumbnailSize();
	
	QPixmap createThumbnail();
	void setThumbnail();
	
	void showEvent(QShowEvent*) override;
	void adjustCheckboxPosition();
	
	void uploadStarted();

	//skyli added
	void showFilePath();

	QString m_path;
	QToolButton* m_image;
	QLabel* m_nameLabel;
	WhiteCheckbox* m_selectionCheckbox;
  
	QToolButton* m_edit;
	QToolButton* m_upload;
	QToolButton* m_delete;
	QLabel* uploadIndicator;

	// EditingWindow* editingWindow = nullptr;
};
