#pragma once

#include "encrypted-file.hpp"
#include <QObject>
#include <QString>
#include <QFile>
#include <QNetworkAccessManager>
#include <QHttpPart>

class	FileUploader : public QObject {
	Q_OBJECT
		
public:
	FileUploader(QString filePath, QWidget *parent = nullptr);
	bool canUpload();
	
	void perform();
	
signals:
	void Finished(QNetworkReply*);
	
private:
	QFile* file;
	QNetworkAccessManager* networkManager;

	EncryptedFile encryptedFile;
	
	QNetworkRequest request();
	QHttpMultiPart* multiPart();
	QHttpPart textPart(QString, QString);
};