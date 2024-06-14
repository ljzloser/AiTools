#include "ConfigDialog.h"
#include <QPluginLoader>
Config& Config::instance()
{
	static Config instance;

	return instance;
}

QJsonObject Config::toJson() const
{
	return QJsonObject{
		{"transparent", transparent},
		{"theme", theme},
		{"autoFill", autoFill},
		{"focusPoint", focusPoint},
		{"focusHide", focusHide},
		{"pointMode", pointMode},
		{"autoCopy", autoCopy},
		{"lastPrompt", lastPrompt},
		{"promptPoint", promptPoint},
		{"keySequence", keySequence},
		{"showTime", showTime},
		{"width", width},
		{"height", height},
		{"aiPlugin", aiPlugin}
	};
}

void Config::fromJson(const QJsonObject& obj, bool init)
{
	auto newObj = obj;
	transparent = newObj.value("transparent").toDouble();
	theme = newObj.value("theme").toInt();
	autoFill = newObj.value("autoFill").toBool();
	focusPoint = newObj.value("focusPoint").toInt();
	focusHide = newObj.value("focusHide").toBool();
	pointMode = newObj.value("pointMode").toInt();
	autoCopy = newObj.value("autoCopy").toBool();
	lastPrompt = newObj.value("lastPrompt").toBool();
	promptPoint = newObj.value("promptPoint").toInt();
	keySequence = newObj.value("keySequence").toString();
	showTime = newObj.value("showTime").toBool();
	if (newObj.contains("aiPlugin"))
		aiPlugin = newObj.value("aiPlugin").toString();
	else
		newObj.insert("aiPlugin", aiPlugin);
	if (newObj.contains("width"))
		width = newObj.value("width").toInt();
	else
		newObj.insert("width", width);
	if (newObj.contains("height"))
		height = newObj.value("height").toInt();
	else
		newObj.insert("height", height);
	if (!init)
		config()->init(QJsonDocument(newObj));
}

LJsonConfig* Config::config()
{
	return _config;
}

BasePlugin* Config::plugin()
{
	QString path = QString("%1/%2/%3/%3.dll").arg(QApplication::applicationDirPath(), "Plugins", aiPlugin);
	QPluginLoader loader(path);
	QObject* plugin = loader.instance();
	if (plugin)
	{
		BasePluginFactory* factory = qobject_cast<BasePluginFactory*>(plugin);
		if (factory)
		{
			BasePlugin* plugin = factory->create();
			plugin->setObjectName(aiPlugin);
			return plugin;
		}
	}
	return nullptr;
}

Config::Config()
{
	if (this->config()->readJson().isNull()) // 初始化防止有的人把config.json删了
		this->config()->init(QJsonDocument(this->toJson()));
	else
		this->fromJson(this->config()->readJson().object(), true);
}

Config::~Config()
{
	_config->init(QJsonDocument(this->toJson()));
	delete _config;
}

ConfigDialog::ConfigDialog(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlag(Qt::Tool);
	QDir dir = QApplication::applicationDirPath() + "/Plugins";
	// 遍历里面的每一个文件夹

	QStringList pluginDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	ui.aIComboBox->addItems(pluginDirs);

	ui.transparentDoubleSpinBox->setValue(Config::instance().transparent);
	ui.themeComboBox->setCurrentIndex(Config::instance().theme);
	ui.autoFillButton->setChecked(Config::instance().autoFill);
	ui.focusPointComboBox->setCurrentIndex(Config::instance().focusPoint);
	ui.fousButton->setChecked(Config::instance().focusHide);
	ui.PointComboBox->setCurrentIndex(Config::instance().pointMode);
	ui.autoCopyButton->setChecked(Config::instance().autoCopy);
	ui.lastPromptButton->setChecked(Config::instance().lastPrompt);
	ui.promptPointComboBox->setCurrentIndex(Config::instance().promptPoint);
	ui.keySequenceEdit->setKeySequence(Config::instance().keySequence);
	ui.showTimeButton->setChecked(Config::instance().showTime);
	ui.aIComboBox->setCurrentText(Config::instance().aiPlugin);

	connect(ui.buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &QDialog::accept);
	connect(ui.buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &QDialog::reject);
	connect(ui.pushButton, &QPushButton::clicked, [=]()
		{
			const QString path = "file:///" + QApplication::applicationDirPath() + "/prompt.json";
			QDesktopServices::openUrl(QUrl(path));
		});
}

ConfigDialog::~ConfigDialog() = default;

void ConfigDialog::accept()
{
	Config::instance().fromJson({
		{"transparent", ui.transparentDoubleSpinBox->value()},
		{"theme", ui.themeComboBox->currentIndex()},
		{"autoFill", ui.autoFillButton->isChecked()},
		{"focusPoint",ui.focusPointComboBox->currentIndex()},
		{"focusHide", ui.fousButton->isChecked()},
		{"pointMode", ui.PointComboBox->currentIndex()},
		{"autoCopy", ui.autoCopyButton->isChecked()},
		{"lastPrompt", ui.lastPromptButton->isChecked()},
		{"promptPoint", ui.promptPointComboBox->currentIndex()},
		{"keySequence", ui.keySequenceEdit->keySequence().toString()},
		{"showTime", ui.showTimeButton->isChecked()},
		{"aiPlugin", ui.aIComboBox->currentText()}
		});
	emit saved(false);
	QDialog::accept();
}