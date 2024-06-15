﻿#pragma once

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
	void initUi();
	void initTratIcon();
	void initConnect();
signals:
	void request(const QString& text);
public slots:
	void reply(const QString& text);
	void loadConfig(bool init = false);
private slots:
	void onHotkeyPressed();
	void systemTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
	void sendMessage() const;
	void openLoginDialog();
	void openSettingDialog();
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
