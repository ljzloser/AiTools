#pragma once
#include "QWidget"
#include <QtPlugin>
class BasePlugin : public QWidget
{
	Q_OBJECT
public:
	explicit BasePlugin(QWidget* parent = nullptr);
	~BasePlugin() override;
	virtual QString getName() = 0;
signals:
	void reply(const QString& text);
	void closed();
public slots:
	/*
	 * 接受用户发送的请求
	 */
	virtual void request(const QString& text) = 0;
	/**
	 * 是否可以发送回复
	 * @param running
	 */
	virtual void setReplyRunning(bool running) = 0;
	/*
	 * 重新加载
	 */
	virtual void load() = 0;
	/*
	 * 发送回复的文本
	 */
	virtual	void setReply(const QString& text);

protected:
	void closeEvent(QCloseEvent* event) override;
};

inline BasePlugin::BasePlugin(QWidget* parent)
	:QWidget(parent)
{
}

inline BasePlugin::~BasePlugin() = default;

inline void BasePlugin::setReply(const QString& text)
{
	emit reply(text);
}

inline void BasePlugin::closeEvent(QCloseEvent* event)
{
	QWidget::closeEvent(event);
	emit closed();
}

class BasePluginFactory
{
public:
	virtual BasePlugin* create() = 0;
	virtual ~BasePluginFactory() = default;
};

#define BasePlugin_IID "org.AiTools.BasePlugin"

Q_DECLARE_INTERFACE(BasePluginFactory, BasePlugin_IID)