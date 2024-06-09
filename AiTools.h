#pragma once

#include <QHotKey>
#include <LWidget>
#include <LCore>
#include <QSystemTrayIcon>
#include <QMenu>
#include "WebDialog.h"
#include <QClipboard>
#include "ConfigDialog.h"
class AiTools : public QWidget
{
	Q_OBJECT

public:
	AiTools(QWidget* parent = nullptr);
	~AiTools();
private:
	void initConnect();

signals:
	void request(const QString& text);
public slots:
	void reply(const QString& text);
	void loadConfig();
private slots:
	void onHotkeyPressed();
	void systemTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
	void sendMessage();
	void openButtonClicked();
private:
	QHotkey* _showHotkey = new QHotkey(this);
	QLineEdit* _inputLineEdit = new QLineEdit(this);
	LTextEdit* _textEdit = new LTextEdit(this);
	QAction* _sendAction = new QAction(this);
	QSystemTrayIcon* _trayIcon = new QSystemTrayIcon(this);
	QMenu* _trayMenu = new QMenu(this);
	QAction* _quitAction = new QAction("退出", this);
	QAction* _bootStrapAction = new QAction("开机启动", this);
	QAction* _loginAction = new QAction("登录", this);
	QAction* _settingAction = new QAction("设置", this);
	QPushButton* _openButton{ new QPushButton(this) };
	WebDialog* _webDialog = { new WebDialog(this) };
	WebDialog* _loginDialog = nullptr;
	LCompleteComboBox* _comboBox = { new LCompleteComboBox(this) };
	QPushButton* _copyButton{ new QPushButton(this) };
	bool _autoFill = true;
	int theme = -1;
	LWidget* _parent = nullptr;
};
