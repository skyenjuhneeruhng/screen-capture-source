#include "window-new-settings.hpp"
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include "volume-control.hpp"

NewSettingsWindow::NewSettingsWindow(QWidget* parent) :
	FramelessWindow(parent),
	m_main(qobject_cast<OBSBasic*>(App()->GetMainWindow()))
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle("Settings");

	//bitrate skyli
	m_bitRates.push_back(120);
	m_bitRates.push_back(350);
	m_bitRates.push_back(700);
	m_bitRates.push_back(1200);
	m_bitRates.push_back(2500);
	m_bitRates.push_back(4000);
	m_bitRates.push_back(5000);

	m_resolutions["720p"] = {1280, 720};
	m_resolutions["1080p"] = {1920, 1080};
	m_resolutions["1440p"] = {2560, 1440};

	m_content = new QWidget(this);
	m_bitRate = new QComboBox(m_content);
	m_resolution = new QComboBox(m_content);
	m_divider = new QWidget(m_content);
	m_username = new QLineEdit(m_content);

	m_webcamSource = nullptr;
	if(obs_scene_find_source(m_main->GetCurrentScene(), "webcam") != nullptr)
	{
		m_webcamSource = new QComboBox(m_content);
		SetupWebcamSources();
		connect(m_webcamSource, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(WebcamSourceChanged(const QString&)));
	}

	m_content->setObjectName("content");
	m_bitRate->setObjectName("bitRate");
	m_resolution->setObjectName("resolution");
	m_divider->setObjectName("divider");
	m_username->setObjectName("username");

	SetupUI();
}

NewSettingsWindow::~NewSettingsWindow()
{
}

void NewSettingsWindow::SetupUI()
{
	m_content->setMaximumWidth(300);

	QFormLayout* layout = new QFormLayout(this);

	layout->setSpacing(15);
	layout->setVerticalSpacing(10);

	layout->addRow("Bit Rate", m_bitRate);
	layout->addRow("Resolution", m_resolution);
	SetupVolumeLabels(layout);
	if(m_webcamSource != nullptr)
	{
		layout->addRow("Webcam", m_webcamSource);
	}
	layout->addRow(m_divider);
	layout->addRow("Member Name", m_username);

	m_content->setLayout(layout);

	SetupBitRateInputs();
	SetupResolutionInputs();
	SetupUserInputs();

	setContent(m_content);

	connect(m_bitRate, SIGNAL(currentIndexChanged(int)), this, SLOT(BitRatesIndexChanged(int)));
	connect(m_resolution, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(ResolutionIndexChanged(const QString&)));
	connect(m_username, SIGNAL(textChanged(const QString&)), this, SLOT(UsernameTextChanged(const QString&)));
}

void NewSettingsWindow::SetupBitRateInputs()
{
	for(auto iter = m_bitRates.begin(); iter != m_bitRates.end(); ++iter)
	{
		m_bitRate->addItem(QString::number(*iter) + " kbps");
	}
	int bitrate = config_get_int(m_main->Config(), "SimpleOutput", "VBitrate");
	m_bitRate->setCurrentText(QString::number(bitrate) + " kbps");
}

void NewSettingsWindow::SetupResolutionInputs()
{
	m_resolution->addItems({"720p", "1080p", "1440p"});
	int w, h;
	w = config_get_int(m_main->Config(), "Video", "BaseCX");
	h = config_get_int(m_main->Config(), "Video", "BaseCY");
	for(auto iter = m_resolutions.begin(); iter != m_resolutions.end(); ++iter)
	{
		QString key = iter.key();
		auto val = iter.value();
		if(val.first == w && val.second == h)
		{
			m_resolution->setCurrentText(key);
		}
	}
}

void NewSettingsWindow::SetupVolumeLabels(QFormLayout* layout)
{
	obs_enum_sources(
		[](void* p, obs_source_t* source)
	{
		// checking if source contains audio...
		uint32_t flags = obs_source_get_output_flags(source);
		if (flags & OBS_SOURCE_AUDIO)
		{
			VolControl *vol = new VolControl(source, false);

			//vol->setContextMenuPolicy(Qt::CustomContextMenu);

			//connect(vol, &QWidget::customContextMenuRequested,
			//	this, &OBSBasic::VolControlContextMenu);
			//connect(vol, &VolControl::ConfigClicked,
			//	this, &OBSBasic::VolControlContextMenu);

			const char* name = obs_source_get_name(source);
			QFormLayout* layout = static_cast<QFormLayout*>(p);
			layout->addRow(name, vol);
		}

		return true;
	}, layout);
}

void NewSettingsWindow::SetupUserInputs()
{
	m_username->setText(encryptedFile.uploadMemberName);
}

void NewSettingsWindow::SetupWebcamSources()
{
	const char* capid = "video_device_id";
	obs_sceneitem_t* item = obs_scene_find_source(m_main->GetCurrentScene(), "webcam");
	obs_source_t* source = obs_sceneitem_get_source(item);
	obs_properties_t* props = obs_get_source_properties(WEBCAM_CAPTURE_ID);
	obs_property* prop = obs_properties_get(props, capid);
	size_t count = obs_property_list_item_count(prop);
	for(size_t i = 0; i < count; ++i)
	{
		const char* txt = obs_property_list_item_string(prop, i);
		m_webcamSource->addItem(txt);
	}
	obs_data_t* setting = obs_source_get_settings(source);
	const char* curSetting = obs_data_get_string(setting, capid);
	if(curSetting != nullptr)
	{
		m_webcamSource->setCurrentText(curSetting);
	}
}

void NewSettingsWindow::BitRatesIndexChanged(int index)
{
	config_set_int(m_main->Config(), "SimpleOutput", "VBitrate", m_bitRates[index]);
	config_save(m_main->Config());
}

void NewSettingsWindow::ResolutionIndexChanged(const QString& index)
{
	QPair<int, int> val = m_resolutions[index];
	config_set_uint(m_main->Config(), "Video", "BaseCX", val.first);
	config_set_uint(m_main->Config(), "Video", "BaseCY", val.second);
	config_save(m_main->Config());
}

void NewSettingsWindow::WebcamSourceChanged(const QString& index)
{
	const char* capid = "video_device_id";
	obs_sceneitem_t* item = obs_scene_find_source(m_main->GetCurrentScene(), "webcam");
	obs_source_t* source = obs_sceneitem_get_source(item);
	obs_data_t* settings = obs_source_get_settings(source);
	obs_data_set_string(settings, capid, index.toStdString().c_str());
	obs_source_update(source, settings);
	obs_data_release(settings);
	m_main->SetupDesktopDisplay();
}

void NewSettingsWindow::ServerTextChanged(const QString& text)
{
	config_set_string(m_main->Config(), "Upload", "Server", text.toStdString().c_str());
	config_save(m_main->Config());
}

void NewSettingsWindow::UsernameTextChanged(const QString& text)
{
	encryptedFile.setUploadMemberName(text);
}

void NewSettingsWindow::PasswordTextChanged(const QString& text)
{
	config_set_string(m_main->Config(), "Upload", "Password", text.toStdString().c_str());
	config_save(m_main->Config());
}

void NewSettingsWindow::closeEvent(QCloseEvent *event)
{
	m_main->isNewSettingsOpened = false;
}