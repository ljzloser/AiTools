#include "ConfigDialog.h"
#include <QPluginLoader>
#include <QDesktopServices>
Config& Config::instance()
{
	static Config instance;

	return instance;
}

QJsonObject Config::toJson() const
{
#define ConfigPair(Key,type,func) {Key.name , Key.value.value<type>()func}
	return QJsonObject{
		ConfigPair(transparent, double),
		ConfigPair(theme, int),
		ConfigPair(autoFill, bool),
		ConfigPair(focusPoint, int),
		ConfigPair(focusHide, bool),
		ConfigPair(pointMode, int),
		ConfigPair(autoCopy, bool),
		ConfigPair(lastPrompt, bool),
		ConfigPair(promptPoint, int),
		ConfigPair(keySequence, QString),
		ConfigPair(showTime, bool),
		ConfigPair(width, int),
		ConfigPair(height, int),
		ConfigPair(pluginInfo, PluginInfo*,->fileName),
	};
#undef ConfigPair
}

void Config::fromJson(const QJsonObject& obj, bool init)
{
#define FSTRING(...) \
    ([&]() { \
        std::ostringstream oss; \
        oss << __VA_ARGS__; \
        return oss.str(); \
    }())

	auto newObj = obj;
	transparent.value = newObj.value(transparent.name).toDouble();
	theme.value = newObj.value(theme.name).toInt();
	autoFill.value = newObj.value(autoFill.name).toBool();
	focusPoint.value = newObj.value(focusPoint.name).toInt();
	focusHide.value = newObj.value(focusHide.name).toBool();
	pointMode.value = newObj.value(pointMode.name).toInt();
	autoCopy.value = newObj.value(autoCopy.name).toBool();
	lastPrompt.value = newObj.value(lastPrompt.name).toBool();
	promptPoint.value = newObj.value(promptPoint.name).toInt();
	keySequence.value = newObj.value(keySequence.name).toString();
	showTime.value = newObj.value(showTime.name).toBool();
	if (newObj.contains(pluginInfo.name))
		pluginInfo.value.value<PluginInfo*>()->fileName = newObj.value(pluginInfo.name).toString();
	else
		newObj.insert(pluginInfo.name, pluginInfo.value.value<PluginInfo*>()->fileName);
	if (newObj.contains(width.name))
		width.value = newObj.value(width.name).toInt();
	else
		newObj.insert(width.name, width.value.toInt());
	if (newObj.contains(height.name))
		height.value = newObj.value(height.name).toInt();
	else
		newObj.insert(height.name, height.value.toInt());
	if (!init)
	{
		config()->init(QJsonDocument(newObj));
	}
}

LJsonConfig* Config::config()
{
	return _config;
}

BasePlugin* Config::plugin()
{
	QString path = QString("%1/%2/%3/%3.dll").arg(QApplication::applicationDirPath(), StrMgr::str.pluginDir, pluginInfo.value.value<PluginInfo*>()->fileName);
	QPluginLoader loader(path);
	QObject* plugin = loader.instance();
	if (plugin)
	{
		BasePluginFactory* factory = qobject_cast<BasePluginFactory*>(plugin);
		if (factory)
		{
			BasePlugin* plugin = factory->create();
			plugin->setObjectName(pluginInfo.value.value<PluginInfo*>()->fileName);
			pluginInfo.value.value<PluginInfo*>()->name = plugin->getName();
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
	delete pluginInfo.value.value<PluginInfo*>();
	_config->init(QJsonDocument(this->toJson()));
	delete _config;
}

ConfigDialog::ConfigDialog(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlag(Qt::Tool);
	QDir dir = LFunc::FString(QApplication::applicationDirPath(), "/", StrMgr::str.pluginDir);
	// 遍历里面的每一个文件夹

	QStringList pluginDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	ui.aIComboBox->addItems(pluginDirs);

	ui.transparentDoubleSpinBox->setValue(Config::instance().transparent.value.toDouble());
	ui.themeComboBox->setCurrentIndex(Config::instance().theme.value.toInt());
	ui.autoFillButton->setChecked(Config::instance().autoFill.value.toBool());
	ui.focusPointComboBox->setCurrentIndex(Config::instance().focusPoint.value.toInt());
	ui.fousButton->setChecked(Config::instance().focusHide.value.toBool());
	ui.PointComboBox->setCurrentIndex(Config::instance().pointMode.value.toInt());
	ui.autoCopyButton->setChecked(Config::instance().autoCopy.value.toBool());
	ui.lastPromptButton->setChecked(Config::instance().lastPrompt.value.toBool());
	ui.promptPointComboBox->setCurrentIndex(Config::instance().promptPoint.value.toInt());
	ui.keySequenceEdit->setKeySequence(Config::instance().keySequence.value.toString());
	ui.showTimeButton->setChecked(Config::instance().showTime.value.toBool());
	ui.aIComboBox->setCurrentText(Config::instance().pluginInfo.value.value<PluginInfo*>()->fileName);

	connect(ui.buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &QDialog::accept);
	connect(ui.buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &QDialog::reject);
	connect(ui.pushButton, &QPushButton::clicked, [=]()
		{
			const QString path = LFunc::FString("file:///", QApplication::applicationDirPath(), "/", StrMgr::str.promptFile);
			QDesktopServices::openUrl(QUrl(path));
		});
}

ConfigDialog::~ConfigDialog() = default;

void ConfigDialog::accept()
{
	Config::instance().fromJson({
	{ StrMgr::str.transparent, ui.transparentDoubleSpinBox->value() },
	{ StrMgr::str.theme, ui.themeComboBox->currentIndex() },
	{ StrMgr::str.autoFill, ui.autoFillButton->isChecked() },
	{ StrMgr::str.focusPoint, ui.focusPointComboBox->currentIndex() },
	{ StrMgr::str.focusHide, ui.fousButton->isChecked() },
	{ StrMgr::str.pointMode, ui.PointComboBox->currentIndex() },
	{ StrMgr::str.autoCopy, ui.autoCopyButton->isChecked() },
	{ StrMgr::str.lastPrompt, ui.lastPromptButton->isChecked() },
	{ StrMgr::str.promptPoint, ui.promptPointComboBox->currentIndex() },
	{ StrMgr::str.keySequence, ui.keySequenceEdit->keySequence().toString() },
	{ StrMgr::str.showTime, ui.showTimeButton->isChecked() },
	{ StrMgr::str.pluginInfo, ui.aIComboBox->currentText() }
		});
	emit saved(false);
	QDialog::accept();
}