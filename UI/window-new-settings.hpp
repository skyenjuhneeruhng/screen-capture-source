#pragma once

#include <QRadioButton>
#include <QSlider>
#include <QCheckBox>
#include <QLineEdit>
#include "deps/frameless-darkstyle/framelesswindow/framelesswindow.h"
#include "window-basic-main.hpp"
#include "features/upload/encrypted-file.hpp"

class NewSettingsWindow : public FramelessWindow {
	Q_OBJECT
public:
	NewSettingsWindow(QWidget *parent = nullptr);
	virtual ~NewSettingsWindow() override;

private:
	void SetupUI();
	void SetupBitRateInputs();
	void SetupResolutionInputs();
	void SetupVolumeLabels(QFormLayout* layout);
	void SetupUserInputs();
	void SetupWebcamSources();

	QList<int> m_bitRates;
	QMap<QString, QPair<int, int>> m_resolutions;

	OBSBasic* m_main;
	QWidget* m_content;
	QComboBox* m_bitRate;
	QComboBox* m_resolution;
	QComboBox* m_webcamSource;
	QWidget* m_divider;
	QLineEdit* m_server;
	QLineEdit* m_username;
	QLineEdit* m_password;
	
	EncryptedFile encryptedFile;

private slots:
	void BitRatesIndexChanged(int index);
	void ResolutionIndexChanged(const QString& index);
	void WebcamSourceChanged(const QString& index);
	void ServerTextChanged(const QString& text);
	void UsernameTextChanged(const QString& text);
	void PasswordTextChanged(const QString& text);
	void closeEvent(QCloseEvent *event);
};
