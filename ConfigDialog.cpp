#include "ConfigDialog.h"
#include <QMessageBox>
ConfigDialog::ConfigDialog(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlag(Qt::Tool);
	QJsonDocument doc = _config->readJson();
	if (doc.isNull())
		_config->init(QJsonDocument::fromJson("{}"));
	else
		_config->init(doc);
	QJsonObject obj = _config->readJson().object();
	if (obj.isEmpty())
	{
		_config->write("transparent", 1);
		_config->write("theme", "¸úËæÏµÍ³");
		_config->write("autoFill", true);
		_config->write("keySequence", "Ctrl+G");
	}
	obj = _config->readJson().object();
	if (!obj.isEmpty())
	{
		ui.transparentDoubleSpinBox->setValue(obj.value("transparent").toDouble());
		ui.themeComboBox->setCurrentIndex(obj.value("theme").toInt());
		ui.autoFillButton->setChecked(obj.value("autoFill").toBool());
		ui.keySequenceEdit->setKeySequence(QKeySequence(obj.value("keySequence").toString()));
	}

	connect(ui.buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &QDialog::accept);
	connect(ui.buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &QDialog::reject);
	connect(ui.pushButton, &QPushButton::clicked, [=]()
		{
			QString path = "file:///" + QApplication::applicationDirPath() + "/prompt.json";
			QDesktopServices::openUrl(QUrl(path));
		});
}

ConfigDialog::~ConfigDialog()
{
	delete _config;
}

void ConfigDialog::accept()
{
	_config->write("transparent", ui.transparentDoubleSpinBox->value());
	_config->write("theme", ui.themeComboBox->currentIndex());
	_config->write("autoFill", ui.autoFillButton->isChecked());
	_config->write("keySequence", ui.keySequenceEdit->keySequence().toString());
	emit saved();
	QDialog::accept();
}