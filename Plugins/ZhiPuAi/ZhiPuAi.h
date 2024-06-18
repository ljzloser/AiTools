#pragma once

#include "zhipuai_global.h"
#include "../../BasePlugin.h"
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWidget>
#include <LCore>

class ZHIPUAI_EXPORT ZhiPuAi : public BasePlugin
{
	Q_OBJECT

public:
	explicit ZhiPuAi(QWidget* parent = nullptr);
	~ZhiPuAi();
	QString getName() override { return "智谱清言"; };
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
	QWebEngineProfile* _profile{ new QWebEngineProfile(QString::fromLatin1("ZhiPuAi.%1").arg(qWebEngineChromiumVersion())) };
	QWebEnginePage* _page{ new QWebEnginePage(_profile) };
	QWebEngineView* _view{ new QWebEngineView() };
	QString _lastHash{ "" };
	QTimer* _timer{ new QTimer(this) };
};

class ZHIPUAI_EXPORT ZhiPuAiPlugin : public QObject, public BasePluginFactory
{
	Q_OBJECT
		Q_PLUGIN_METADATA(IID BasePlugin_IID)
		Q_INTERFACES(BasePluginFactory)
public:
	BasePlugin* create() override { return new ZhiPuAi(); };
	~ZhiPuAiPlugin() override = default;
};
