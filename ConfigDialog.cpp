#include "ConfigDialog.h"

Config& Config::instance()
{
	static Config instance;
	if (instance.config()->readJson().isNull())
		instance.config()->init(QJsonDocument(instance.toJson()));
	else
		instance.fromJson(instance.config()->readJson().object(), true);
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
	};
}

void Config::fromJson(const QJsonObject& obj, bool init)
{
	transparent = obj.value("transparent").toDouble();
	theme = obj.value("theme").toInt();
	autoFill = obj.value("autoFill").toBool();
	focusPoint = obj.value("focusPoint").toInt();
	focusHide = obj.value("focusHide").toBool();
	pointMode = obj.value("pointMode").toInt();
	autoCopy = obj.value("autoCopy").toBool();
	lastPrompt = obj.value("lastPrompt").toBool();
	promptPoint = obj.value("promptPoint").toInt();
	keySequence = obj.value("keySequence").toString();
	showTime = obj.value("showTime").toBool();
	if (!init)
		config()->init(QJsonDocument(obj));
}

LJsonConfig* Config::config()
{
	return _config;
}

Config::~Config()
{
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
	emit saved();
	QDialog::accept();
}