#include "file-uploader.hpp"
#include <QFileInfo>
#include <QMessageBox>


FileUploader::FileUploader(QString filePath, QWidget *parent) : QObject() {
	file = new QFile(filePath);
	networkManager = new QNetworkAccessManager(this);
	
}

bool FileUploader::canUpload() {
	return encryptedFile.readSuccessfully();
}


void FileUploader::perform() {
	connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SIGNAL(Finished(QNetworkReply*)));
	QNetworkReply *reply = networkManager->post(request(), multiPart());
}


QNetworkRequest FileUploader::request() {
	return QNetworkRequest(QUrl(encryptedFile.uploadUrl));
}

QHttpMultiPart* FileUploader::multiPart() {
	QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	
	multiPart->append(textPart("uploadUserName", encryptedFile.uploadUserName));
	multiPart->append(textPart("uploadUserPassword", encryptedFile.uploadUserPassword));
	multiPart->append(textPart("uploadMemberName", encryptedFile.uploadMemberName));

	file->open(QIODevice::ReadOnly);
	file->setParent(multiPart);

	QHttpPart filePart;
	filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("video/mp4"));
	QString nameHeader = QString("form-data; name=\"uploadUserFile\"; filename=\"%1\"").arg(QFileInfo(file->fileName()).fileName());
	filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(nameHeader));
	filePart.setBodyDevice(file);
	multiPart->append(filePart);
	
	return multiPart;
}

QHttpPart FileUploader::textPart(QString name, QString value) {
	QHttpPart httpPart;
	QString nameHeader = QString("form-data; name=\"%1\"").arg(name);
	httpPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(nameHeader));
	httpPart.setBody(value.toLocal8Bit());
	return httpPart;
}
