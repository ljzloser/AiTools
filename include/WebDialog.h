#pragma once

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWidget>
#include <LCore>
#include "BasePlugin.h"
#include "StringManager.h"

class WebDialog : public BasePlugin
{
	Q_OBJECT
public:
	explicit WebDialog(QWidget* parent = nullptr);
	~WebDialog() override;
	[[nodiscard]] QWebEngineView* view() const { return _view; }
	QString getName() override { return "WebDialog"; }
	QString getUrl() override { return "https://www.baidu.com"; }
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
	QWebEngineProfile* _profile{ new QWebEngineProfile(QString::fromLatin1("%1.%2").arg(StrMgr::str.appName,qWebEngineChromiumVersion())) };
	QWebEnginePage* _page{ new QWebEnginePage(_profile) };
	QWebEngineView* _view{ new QWebEngineView() };
	QString _lastHash{ "" };
	QTimer* _timer{ new QTimer(this) };
};
