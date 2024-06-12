#include "ConfigDialog.h"

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
		{"height", height}
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

	const QJsonObject obj = Config::instance().toJson();
	if (!obj.isEmpty())
	{
		ui.transparentDoubleSpinBox->setValue(obj.value("transparent").toDouble());
		ui.themeComboBox->setCurrentIndex(obj.value("theme").toInt());
		ui.autoFillButton->setChecked(obj.value("autoFill").toBool());
		ui.focusPointComboBox->setCurrentIndex(obj.value("focusPoint").toInt());
		ui.fousButton->setChecked(obj.value("focusHide").toBool());
		ui.PointComboBox->setCurrentIndex(obj.value("pointMode").toInt());
		ui.autoCopyButton->setChecked(obj.value("autoCopy").toBool());
		ui.lastPromptButton->setChecked(obj.value("lastPrompt").toBool());
		ui.promptPointComboBox->setCurrentIndex(obj.value("promptPoint").toInt());
		ui.keySequenceEdit->setKeySequence(obj.value("keySequence").toString());
		ui.showTimeButton->setChecked(obj.value("showTime").toBool());
	}

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
		});
	emit saved(false);
	QDialog::accept();
}