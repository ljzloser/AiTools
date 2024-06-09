#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QUrl>
#include <QTextEdit>
#include <QTimer>
#include <QCryptographicHash>
#include <QThread>
#include <LCore>
class WebDialog : public QWidget
{
	Q_OBJECT

public:
	WebDialog(QWidget* parent = nullptr);
	~WebDialog();
	QWebEngineView* view() { return _view; }
signals:
	void reply(const QString& text);
	void closed();
public slots:
	void request(const QString& text);
	void setReplyRunning(bool running);
	void load();
private slots:
	void timeouted();

private:
	QWebEngineProfile* _profile{ new QWebEngineProfile(QString::fromLatin1("AiTools.%1").arg(qWebEngineChromiumVersion())) };
	QWebEnginePage* _page{ new QWebEnginePage(_profile) };
	QWebEngineView* _view{ new QWebEngineView() };
	QString _lastHash{ "" };
	QTimer* _timer{ new QTimer(this) };
	void closeEvent(QCloseEvent* event) override;
};
