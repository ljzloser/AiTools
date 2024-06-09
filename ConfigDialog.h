#pragma once

#include <QDialog>
#include "ui_ConfigDialog.h"
#include <LCore>
#include <QDesktopServices>

class ConfigDialog final : public QDialog
{
	Q_OBJECT
signals:
	void saved();
public:
	explicit ConfigDialog(QWidget* parent = nullptr);
	~ConfigDialog() override;
	void accept() override;
private:
	Ui::ConfigDialogClass ui;
	LJsonConfig* _config{ new LJsonConfig(QApplication::applicationDirPath() + "/config.json") };
};
