#pragma once

#include <QHotKey>
#include <LWidget>
#include <LCore>
#include <QSystemTrayIcon>
#include <QMenu>
#include "WebDialog.h"
#include "ConfigDialog.h"

class TitleBar final :public LTitleBar
{
public:
	explicit TitleBar(QWidget* parent = nullptr);
	bool showTime = true;
protected:
	void paintEvent(QPaintEvent* event) override;
};
class Widget final : public LWidget
{
	Q_OBJECT
public:
	Widget(LBaseTitleBar* titleBar, QWidget* mainWidget, QWidget* parent = nullptr);
	void changeEvent(QEvent* event) override;
	bool focusHide = false;
};

class AiTools final : public QWidget
{
	Q_OBJECT
public:
	explicit AiTools(QWidget* parent = nullptr);
	~AiTools() override;
private:
	void initUi();
	void initTratIcon();
	void initConnect();
signals:
	void request(const QString& text);
public slots:
	void reply(const QString& text) const;
	void loadConfig();
private slots:
	void onHotkeyPressed() const;
	void systemTrayIconActivated(QSystemTrayIcon::ActivationReason reason) const;
	void sendMessage() const;
	void openLoginDialog();
	void openSettingDialog();
private:
	QHotkey* _showHotkey{ new QHotkey(QKeySequence(), true, this) };
	QLineEdit* _inputLineEdit{ new QLineEdit(this) };
	LCompleteComboBox* _comboBox{ new LCompleteComboBox(this) };
	LTextEdit* _textEdit{ new LTextEdit(this) };
	QSystemTrayIcon* _trayIcon{ new QSystemTrayIcon(this) };
	QMenu* _trayMenu{ new QMenu(this) };
	QAction* _sendAction{ new QAction(this) };
	QAction* _quitAction{ new QAction("退出", this) };
	QAction* _bootStrapAction{ new QAction("开机启动", this) };
	QAction* _loginAction{ new QAction("登录", this) };
	QAction* _settingAction{ new QAction("设置", this) };
	WebDialog* _webDialog{ new WebDialog(this) };
	WebDialog* _loginDialog{ nullptr };
	Widget* _parent{ nullptr };
	QPushButton* _openButton{ new QPushButton(this) };
	QPushButton* _copyButton{ new QPushButton(this) };
	QPushButton* _clearButton{ new QPushButton(this) };
	bool _autoFill{ true };
	int _theme{ -1 };
	int _pointMode{ 0 };
};
