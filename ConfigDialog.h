#pragma once

#include <QDialog>
#include "ui_ConfigDialog.h"
#include <LCore>
#include <QDesktopServices>
class ConfigDialog : public QDialog
{
	Q_OBJECT
signals:
	void saved();
public:
	ConfigDialog(QWidget* parent = nullptr);
	~ConfigDialog();
	void accept() override;
private:
	Ui::ConfigDialogClass ui;
	LJsonConfig* _config = new LJsonConfig(QApplication::applicationDirPath() + "/config.json");
};
