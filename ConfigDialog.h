#pragma once

#include <QDialog>
#include "ui_ConfigDialog.h"
#include "BasePlugin.h"
#include <LCore>
#include <QDesktopServices>

struct PluginInfo
{
	QString fileName;
	QString name;
	PluginInfo() = default;
	PluginInfo(const QString& fileName, const QString& name) : fileName(fileName), name(name) {}
	QString toString() const { return QString("%1;%2").arg(fileName, name); }
};
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
	double width = 300;
	double height = 200;
	PluginInfo pluginInfo = { "ZhiPuAi", "智谱清言" };
	static Config& instance();
	[[nodiscard]] QJsonObject toJson() const;
	void fromJson(const QJsonObject& obj, bool init = false);
	LJsonConfig* config();
	BasePlugin* plugin();

private:
	Config();
	~Config();
	LJsonConfig* _config{ new LJsonConfig(QApplication::applicationDirPath() + "/config.json") };
};

class ConfigDialog final : public QDialog
{
	Q_OBJECT
signals:
	void saved(bool init = false);
public:
	explicit ConfigDialog(QWidget* parent = nullptr);
	~ConfigDialog() override;
	void accept() override;
private:
	Ui::ConfigDialogClass ui;
};
