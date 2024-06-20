#pragma once

#include "ChatGpt_global.h"
#include "../../BasePlugin.h"
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWidget>
#include <LCore>

class ChatGpt_EXPORT ChatGpt : public BasePlugin
{
	Q_OBJECT

public:
	explicit ChatGpt(QWidget* parent = nullptr);
	~ChatGpt();
	QString getName() override { return "ChatGpt"; }
	QString getUrl() override { return _url.toString(); }
	QIcon getIcon() override { return QIcon(":/icon/ChatGpt.ico"); };
	[[nodiscard]] QWebEngineView* view() const { return _view; }
signals:
	void reply(const QString& text);
	void closed();
public slots:
	void request(const QString& text, bool running = true) override;
	void setReplyRunning(bool running) override;
	void load() override;
private slots:
	void timeouted();

private:
	QWebEngineProfile* _profile{ new QWebEngineProfile(QString::fromLatin1("ChatGpt.%1").arg(qWebEngineChromiumVersion())) };
	QWebEnginePage* _page{ new QWebEnginePage(_profile) };
	QWebEngineView* _view{ new QWebEngineView() };
	QString _lastHash{ "" };
	QTimer* _timer{ new QTimer(this) };
	QUrl _url{ "https://chat.openai.com/" };
};

class ChatGpt_EXPORT ChatGptPlugin : public QObject, public BasePluginFactory
{
	Q_OBJECT
		Q_PLUGIN_METADATA(IID BasePlugin_IID)
		Q_INTERFACES(BasePluginFactory)
public:
	BasePlugin* create() override { return new ChatGpt(); };
	~ChatGptPlugin() override = default;
};
