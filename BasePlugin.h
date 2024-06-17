#pragma once
#include "QWidget"
// 插件基类
class BasePlugin : public QWidget
{
	Q_OBJECT
public:
	explicit BasePlugin(QWidget* parent = nullptr);
	~BasePlugin() override;
	/**
	 * @brief 获取插件名称
	 * @return 
	 */
	virtual QString getName() = 0;
signals:
	// 回复信号
	void reply(const QString& text);
	// 窗口关闭信号
	void closed();
public slots:
	// 接受用户发送的请求
	virtual void request(const QString& text) = 0;
	/**
	 * @brief 是否可以发送回复
	 * @param running
	 */
	virtual void setReplyRunning(bool running) = 0;
	/*
	 * @brief 重新加载
	 */
	virtual void load() = 0;
	/*
	 *@brief 发送回复的文本
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
// 插件工厂
class BasePluginFactory
{
public:
	/**
	 * @brief 创建插件
	 * @return 
	 */
	virtual BasePlugin* create() = 0;
	virtual ~BasePluginFactory() = default;
};

#define BasePlugin_IID "org.AiTools.BasePlugin"

Q_DECLARE_INTERFACE(BasePluginFactory, BasePlugin_IID)