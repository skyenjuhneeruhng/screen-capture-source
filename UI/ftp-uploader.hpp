#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QProgressBar>
#include "deps/frameless-darkstyle/framelesswindow/framelesswindow.h"

class FtpUploader : public QObject {
Q_OBJECT
public:
	FtpUploader(QString server, QString username, QString password);
	~FtpUploader();

	void UploadFile(QString& file);

private slots:
	void UploadProgress(qint64 read, qint64 total);
	void UploadFinished(QNetworkReply* reply);

private:
	QString m_server;
	QString m_username;
	QString m_password;
	QNetworkAccessManager* m_nam;
	FramelessWindow* m_uploadDialog;
	QProgressBar* m_pb;
};
