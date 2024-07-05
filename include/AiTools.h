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
	Q_OBJECT
public:
	explicit TitleBar(QWidget* parent = nullptr);
	~TitleBar() override = default;
protected:
	void paintEvent(QPaintEvent* event) override;
};
class Widget final : public LWidget
{
	Q_OBJECT
public:
	explicit Widget(LBaseTitleBar* titleBar, QWidget* mainWidget, QWidget* parent = nullptr);
	~Widget() override = default;
protected:
	void changeEvent(QEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void showEvent(QShowEvent* event) override;
private:
	LAnimationOpacityEffect* _effect = new LAnimationOpacityEffect(this);
};

class AiTools final : public QWidget
{
	Q_OBJECT
public:
	explicit AiTools(QWidget* parent = nullptr);
	~AiTools() override;
private:
	/**
	 * 初始化Ui界面
	 */
	void initUi();
	/**
	 * 初始化托盘图标
	 */
	void initTratIcon();
	/**
	 * 初始化信号和槽
	 */
	void initConnect();
signals:
	/**
	 * 向插件发送用户输入的文本信号
	 * @param text 文本
	 */
	void request(const QString& text);
public slots:
	/**
	 * 插件返回的结果文本的槽函数
	 * @param text 文本
	 */
	void reply(const QString& text);
	/**
	 * 加载配置
	 * @param init 是否初始化加载
	 */
	void loadConfig(bool init = false);
private slots:
	/**
	 * 全局热键点击槽函数，用于显示和隐藏对话界面
	 */
	void onHotkeyPressed();
	/**
	 * 右下角系统托盘图标激活槽函数
	 * @param reason 激活原因
	 */
	void systemTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
	/**
	 * 向插件发送文本槽函数
	 */
	void sendMessage() const;
	/**
	 * 打开插件登录对话框
	 */
	void openLoginDialog();
	/**
	 * 打开设置对话框
	 */
	void openSettingDialog();
	/**
	 * 调试按钮点击槽函数
	 */
	void debugButtonClicked();
	/**
	 * 打开关于链接
	 */
	void openAboutLink();
	/**
	 * 打开提示文件
	 */
	void openPromptFile();
private:
	QHotkey* _showHotkey{ new QHotkey(QKeySequence(), true, this) };
	QHotkey* _showLoginHotKey{ new QHotkey(QKeySequence(), true, this) };
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
	QAction* _aboutAction{ new QAction(StrMgr::str.about, this) };
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
