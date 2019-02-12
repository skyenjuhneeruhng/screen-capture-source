#pragma once

#include <QString>
#include <QFile>
#include <QSettings>

// Encryption algorithm: AES with 256bit key in CBC mode
#define AES_KEY "j6Doupigc3o99VsspTfjiq53FpboQe5p"
#define AES_IV "oWmeifg1daE25a63"

class EncryptedFile {
public:
  EncryptedFile();

  bool readSuccessfully();

	QString uploadUrl;
	QString uploadUserName;
	QString uploadUserPassword;
	QString uploadMemberName;
	
	void setUploadMemberName(QString value);

private:
	QSettings* settings;
	
	void readFile();
  bool m_readSuccessfully;
	
	QByteArray decrypt(QByteArray);
	QString fileContents;
	
	QString findValue(QString);
	
	void encryptFile(); // used for testing
};