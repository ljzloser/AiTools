#include "ConfigDialog.h"

ConfigDialog::ConfigDialog(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlag(Qt::Tool);
	if (_config->readJson().isNull())
	{
		_config->init(QJsonDocument({
			{"transparent", 1},
			{"theme", 2},
			{"autoFill", true},
			{"keySequence", "Ctrl+G"},
			{"focusHide", false},
			{"pointMode", 0},
			{"showTime", true},
			{"lastPrompt",true},
			{"promptPoint",0},
			}));
	}
	const QJsonObject obj = _config->readJson().object();
	if (!obj.isEmpty())
	{
		ui.transparentDoubleSpinBox->setValue(obj.value("transparent").toDouble());
		ui.themeComboBox->setCurrentIndex(obj.value("theme").toInt());
		ui.autoFillButton->setChecked(obj.value("autoFill").toBool());
		ui.keySequenceEdit->setKeySequence(obj.value("keySequence").toString());
		ui.fousButton->setChecked(obj.value("focusHide").toBool());
		ui.PointComboBox->setCurrentIndex(obj.value("pointMode").toInt());
		ui.showTimeButton->setChecked(obj.value("showTime").toBool());
		ui.lastPromptButton->setChecked(obj.value("lastPrompt").toBool());
		ui.promptPointComboBox->setCurrentIndex(obj.value("promptPoint").toInt());
	}

	connect(ui.buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &QDialog::accept);
	connect(ui.buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &QDialog::reject);
	connect(ui.pushButton, &QPushButton::clicked, [=]()
		{
			const QString path = "file:///" + QApplication::applicationDirPath() + "/prompt.json";
			QDesktopServices::openUrl(QUrl(path));
		});
}

ConfigDialog::~ConfigDialog()
{
	delete _config;
}

void ConfigDialog::accept()
{
	_config->init(QJsonDocument({
		{"transparent", ui.transparentDoubleSpinBox->value()},
		{"theme", ui.themeComboBox->currentIndex()},
		{"autoFill", ui.autoFillButton->isChecked()},
		{"keySequence", ui.keySequenceEdit->keySequence().toString()},
		{"focusHide", ui.fousButton->isChecked()},
		{"pointMode", ui.PointComboBox->currentIndex()},
		{"showTime", ui.showTimeButton->isChecked()},
		{"lastPrompt", ui.lastPromptButton->isChecked()},
		{"promptPoint", ui.promptPointComboBox->currentIndex()},
		}));
	emit saved();
	QDialog::accept();
}