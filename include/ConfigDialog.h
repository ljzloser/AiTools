#pragma once

#include <QDialog>
#include "ui_ConfigDialog.h"
#include "BasePlugin.h"
#include <LCore>
#include <QProxyStyle>
#include "StringManager.h"

// 插件信息
struct PluginInfo
{
	QString fileName;	// 文件名
	QString name;		// 插件名
	PluginInfo() = default;
	PluginInfo(const QString& fileName, const QString& name) : fileName(fileName), name(name) {}
	QString toString() const { return QString("%1;%2").arg(fileName, name); }
};
Q_DECLARE_METATYPE(PluginInfo*)
class Config
{
	// 字段
	struct Field
	{
		QString name;	// 字段名
		QVariant value;	// 字段值
		Field(const QString& name, const QVariant& value) : name(name), value(value) {}
		/*
		 * 给字段赋值，如果obj中没有该字段，那么就将Field的值赋给obj
		 * @param obj json对象
		 */
		void setValue(QJsonObject& obj);
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
	FIELD(aiUrlKeySequence, StrMgr::str.defaultAiUrlKeySequence);
	FIELD(font, QFont(StrMgr::str.defaultFont));
	FIELD(fontSize, 15);
	FIELD(loginDialogWidth, 1600);
	FIELD(loginDialogHeight, 900);
#undef FIELD

	/**
	 * 返回配置实例
	 * @return 配置实例
	 */
	static Config& instance();
	/**
	 * 将配置转为json对象
	 * @return json对象
	 */
	[[nodiscard]] QJsonObject toJson() const;
	/**
	 * 从json对象中恢复配置
	 * @param obj json对象
	 * @param init 是否初始化
	 */
	void fromJson(const QJsonObject& obj, bool init = false);
	/**
	 * 返回配置文件指针
	 * @return 配置文件
	 */
	LJsonConfig* config();
	/**
	 * 返回当前选择的插件
	 * @return 插件
	 */
	BasePlugin* plugin();

private:
	Config();
	~Config();
	LJsonConfig* _config{ new LJsonConfig(QDir(QApplication::applicationDirPath()).filePath(StrMgr::str.configFile)) };
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
class HorTabStyle : public QProxyStyle
{
	Q_OBJECT
public:
	explicit HorTabStyle(const Qt::Orientation orientation = Qt::Horizontal) : _orientation(orientation) {}
	void drawItemText(QPainter* painter, const QRect& rect, int flags, const QPalette& pal, bool enabled, const QString& text, QPalette::ColorRole textRole) const override;
	void drawHorItemText(QPainter* painter, QRect rect, int flags, QString text) const;
private:
	Qt::Orientation _orientation = Qt::Horizontal;
};
