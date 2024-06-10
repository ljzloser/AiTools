#pragma once

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWidget>
#include <LCore>

class WebDialog : public QWidget
{
	Q_OBJECT
public:
	explicit WebDialog(QWidget* parent = nullptr);
	~WebDialog() override;
	[[nodiscard]] QWebEngineView* view() const { return _view; }
signals:
	void reply(const QString& text);
	void closed();
public slots:
	void request(const QString& text) const;
	void setReplyRunning(bool running) const;
	void load() const;
private slots:
	void timeouted();

private:
	QWebEngineProfile* _profile{ new QWebEngineProfile(QString::fromLatin1("AiTools.%1").arg(qWebEngineChromiumVersion())) };
	QWebEnginePage* _page{ new QWebEnginePage(_profile) };
	QWebEngineView* _view{ new QWebEngineView() };
	QString _lastHash{ "" };
	QTimer* _timer{ new QTimer(this) };
protected:
	void closeEvent(QCloseEvent* event) override;
};
