#pragma once

#include <QDialog>
#include "ui_ConfigDialog.h"
#include "BasePlugin.h"
#include <LCore>
#include "StringManager.h"
struct PluginInfo
{
	QString fileName;
	QString name;
	PluginInfo() = default;
	PluginInfo(const QString& fileName, const QString& name) : fileName(fileName), name(name) {}
	QString toString() const { return QString("%1;%2").arg(fileName, name); }
};
Q_DECLARE_METATYPE(PluginInfo)
class Config
{
private:
	struct Field
	{
		QString name;
		QVariant value;
		Field(const QString& name, const QVariant& value) : name(name), value(value) {}
	};
public:
#define FIELD(name,val) Field name = { StrMgr::str.name, val }
	FIELD(transparent, 1.0);
	FIELD(theme, 2);
	FIELD(autoFill, true);
	FIELD(focusPoint, 0);
	FIELD(focusHide, false);
	FIELD(pointMode, 0);
	FIELD(autoCopy, true);
	FIELD(lastPrompt, true);
	FIELD(promptPoint, 0);
	FIELD(keySequence, StrMgr::str.defaultKeySequence);
	FIELD(showTime, true);
	FIELD(width, 300);
	FIELD(height, 200);
	FIELD(pluginInfo, QVariant::fromValue(new PluginInfo(StrMgr::str.defaultPluginFile, StrMgr::str.defaultPluginName)));
	FIELD(autoUpdate, true);
#undef FIELD

	static Config& instance();
	[[nodiscard]] QJsonObject toJson() const;
	void fromJson(const QJsonObject& obj, bool init = false);
	LJsonConfig* config();
	BasePlugin* plugin();

private:
	Config();
	~Config();
	LJsonConfig* _config{ new LJsonConfig(QApplication::applicationDirPath() + "/" + StrMgr::str.configFile) };
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
