#pragma once

#include <QDialog>
#include "ui_ConfigDialog.h"
#include <LCore>
#include <QDesktopServices>
class Config
{
public:
	double transparent = 1;
	int theme = 2;
	bool autoFill = true;
	int focusPoint = 0;
	bool focusHide = false;
	int pointMode = 0;
	bool autoCopy = true;
	bool lastPrompt = true;
	int promptPoint = 0;
	QString keySequence = "Ctrl+G";
	bool showTime = true;
	static Config& instance();

	[[nodiscard]] QJsonObject toJson() const;

	void fromJson(const QJsonObject& obj, bool init = false);
	LJsonConfig* config();

private:
	Config() = default;
	~Config();
	LJsonConfig* _config{ new LJsonConfig(QApplication::applicationDirPath() + "/config.json") };
};

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
};
