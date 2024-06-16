#pragma once

#include <QHotKey>
#include <LWidget>
#include <LCore>
#include <QSystemTrayIcon>
#include <QMenu>
#include "WebDialog.h"
#include "ConfigDialog.h"
#include "StringManager.h"
class TitleBar final :public LTitleBar
{
public:
	explicit TitleBar(QWidget* parent = nullptr);
protected:
	void paintEvent(QPaintEvent* event) override;
};
class Widget final : public LWidget
{
	Q_OBJECT
public:
	Widget(LBaseTitleBar* titleBar, QWidget* mainWidget, QWidget* parent = nullptr);
	void changeEvent(QEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
};

class AiTools final : public QWidget
{
	Q_OBJECT
public:
	explicit AiTools(QWidget* parent = nullptr);
	~AiTools() override;
private:
	/**
	 * @brief 初始化Ui
	 */
	void initUi();
	/**
	 * @brief 初始化托盘图标
	 */
	void initTratIcon();
	/**
	 * @brief 初始化信号槽
	 */
	void initConnect();
signals:
	/**
	 * @brief 向Ai插件发送请求
	 */
	void request(const QString& text);
public slots:
	/**
	 * @brief 接收Ai插件的回复
	 */
	void reply(const QString& text);
	/**
	 * @brief 加载配置
	 * @param init 是否初始化
	 */
	void loadConfig(bool init = false);
private slots:
	/**
	 * @brief 全局热键激活槽函数用于显示主界面
	 */
	void onHotkeyPressed();
	/**
	 * @brief 托盘图标激活槽函数
	 * @param reason 激活原因
	 */
	void systemTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
	/**
	 * @brief 向Ai插件发送消息
	 */
	void sendMessage() const;
	/**
	 * @brief 打开登录对话框
	 */
	void openLoginDialog();
	/**
	 * @brief 打开设置对话框
	 */
	void openSettingDialog();
	/**
	 * @brief 调试按钮点击槽函数
	 */
	void debugButtonClicked();
private:
	QHotkey* _showHotkey{ new QHotkey(QKeySequence(), true, this) };
	QLineEdit* _inputLineEdit{ new QLineEdit(this) };
	LCompleteComboBox* _promptComboBox{ new LCompleteComboBox(this) };
	LTextEdit* _textEdit{ new LTextEdit(this) };
	QSystemTrayIcon* _trayIcon{ new QSystemTrayIcon(this) };
	QMenu* _trayMenu{ new QMenu(this) };
	QAction* _sendAction{ new QAction(this) };
	QAction* _quitAction{ new QAction(StrMgr::str.quit, this) };
	QAction* _bootStrapAction{ new QAction(StrMgr::str.bootStart, this) };
	QAction* _loginAction{ new QAction(StrMgr::str.login, this) };
	QAction* _settingAction{ new QAction(StrMgr::str.setting, this) };
	QAction* _updatePromptAction{ new QAction(StrMgr::str.updatePrompt, this) };
	BasePlugin* _webDialog{ new WebDialog() };
	BasePlugin* _loginDialog{ nullptr };
	ConfigDialog* _configDialog{ nullptr };
	Widget* _parent{ nullptr };
	QPushButton* _openButton{ new QPushButton(this) };
	QPushButton* _copyButton{ new QPushButton(this) };
	QPushButton* _clearButton{ new QPushButton(this) };
	QPushButton* _settingButton{ new QPushButton(this) };
	QPushButton* _debugButton{ new QPushButton(this) };
	bool _isdark = false;
};
