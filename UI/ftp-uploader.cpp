#include "ftp-uploader.hpp"
#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QNetworkReply>
#include <QMessageBox>

#define PROGRESS_SPLIT 10000

FtpUploader::FtpUploader(QString server, QString username, QString password)
	: QObject()
{
	m_server = server;
	m_username = username;
	m_password = password;
	m_nam = new QNetworkAccessManager();
	connect(m_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(UploadFinished(QNetworkReply*)));
}

FtpUploader::~FtpUploader()
{
	delete m_nam;
}

void FtpUploader::UploadFile(QString& file)
{
	m_uploadDialog = new FramelessWindow;
	m_uploadDialog->setAttribute(Qt::WA_DeleteOnClose, true);
	m_pb = new QProgressBar(m_uploadDialog);
	m_pb->setMinimum(0);
	m_pb->setMaximum(PROGRESS_SPLIT);
	m_pb->setTextVisible(false);
	m_uploadDialog->setWindowTitle("Uploading...");
	m_uploadDialog->setContent(m_pb);
	m_uploadDialog->setContentsMargins(0, 15, 0, 15);

	QFile* data = new QFile(file);
	data->deleteLater();

	QUrl uploadUrl(m_server + "/" + QFileInfo(*data).fileName());
	uploadUrl.setUserName(m_username);
	uploadUrl.setPassword(m_password);
	QNetworkRequest upload(uploadUrl);

	if(data->open(QFile::ReadOnly))
	{
		QNetworkReply* reply = m_nam->put(upload, data);
		connect(reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(UploadProgress(qint64, qint64)));
		m_uploadDialog->exec();
	}

}

void FtpUploader::UploadFinished(QNetworkReply* reply)
{
	if(reply->error())
	{
		QMessageBox* mb = new QMessageBox(
			QMessageBox::Icon::Critical,
			"Upload Error",
			reply->errorString()
		);
		mb->exec();
		delete mb;
	}
	m_uploadDialog->close();
}

void FtpUploader::UploadProgress(qint64 read, qint64 total)
{
	total = total == 0 ? 1 : total;
	m_pb->setValue(((double)read / total) * PROGRESS_SPLIT);
}

#undef PROGRESS_SPLIT
