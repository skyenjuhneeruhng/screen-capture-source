#include "encrypted-file.hpp"
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>
#include "Qt-AES/qaesencryption.h"

EncryptedFile::EncryptedFile() {
	settings = new QSettings("screencap", "screencap");
	uploadMemberName = settings->value("uploadMemberName").toString();
		
	readFile();
	uploadUrl = findValue("uploadUrl");
	uploadUserName = findValue("uploadUserName");
	uploadUserPassword = findValue("uploadUserPassword");
	
	// qDebug() << "EncryptedFile" << fileContents;
	// qDebug() << "EncryptedFile" << uploadUrl << uploadUserName << uploadUserPassword;
}

bool EncryptedFile::readSuccessfully() {
  return m_readSuccessfully;
}


void EncryptedFile::readFile() {
	QFile* file = new QFile("uploadAccount.xml");
	m_readSuccessfully = file->open(QFile::ReadOnly);
	
	QByteArray decrypted = decrypt(file->readAll());
	fileContents = QString::fromLatin1(decrypted.data());
	
	file->close();
	delete file;
}


void EncryptedFile::encryptFile() {
	QFile* plainFile = new QFile("uploadAccount.xml.plain");
	plainFile->open(QFile::ReadOnly);
	QFile* encFile = new QFile("uploadAccount.xml");
	encFile->open(QFile::WriteOnly);
	
	QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
	encFile->write( encryption.encode(plainFile->readAll(), AES_KEY, AES_IV) );
	encFile->close();
}

QByteArray EncryptedFile::decrypt(QByteArray rawText) {
	QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
	return encryption.decode(rawText, AES_KEY, AES_IV);
}


QString EncryptedFile::findValue(QString name) {
	QString regex = QString("name=\"%1\" value=\"(\\S+)\"").arg(name);
	return QRegularExpression(regex).match(fileContents).captured(1);
}


void EncryptedFile::setUploadMemberName(QString value) {
	uploadMemberName = value;
	settings->setValue("uploadMemberName", value);
}