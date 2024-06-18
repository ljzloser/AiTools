#pragma once

#include "tongyiai_global.h"

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWidget>
#include <QTimer>
#include "../../BasePlugin.h"

class TONGYIAI_EXPORT TongYiAi : public BasePlugin
{
	Q_OBJECT
public:
	explicit TongYiAi(QWidget* parent = nullptr);
	~TongYiAi() = default;
	QString getName() override { return "通义千问"; }
public slots:
	void request(const QString& text, bool running = true) override;
	void setReplyRunning(bool running) override;
	void setReply(const QString& text) override;
	void load() override;
private slots:
	void timeouted();
protected:
	QWebEngineProfile* _profile{ new QWebEngineProfile(QString::fromLatin1("TongYiAi.%1").arg(qWebEngineChromiumVersion())) };
	QWebEnginePage* _page{ new QWebEnginePage(_profile) };
	QWebEngineView* _view{ new QWebEngineView() };
	QString _lastHash{ "" };
	QTimer* _timer{ new QTimer(this) };
	QUrl _url{ QUrl("https://tongyi.aliyun.com/qianwen/?spm=5176.28326591.0.0.40f76ee1zBYunq") };
};

class TONGYIAI_EXPORT TongYiAiPlugin : public QObject, public BasePluginFactory
{
	Q_OBJECT
		Q_PLUGIN_METADATA(IID BasePlugin_IID)
		Q_INTERFACES(BasePluginFactory)
public:
	BasePlugin* create() override { return new TongYiAi(); }
	~TongYiAiPlugin() override = default;
};
