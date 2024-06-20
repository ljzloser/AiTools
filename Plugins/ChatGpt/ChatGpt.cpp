#include "ChatGpt.h"
#include <QVBoxLayout>
#include <QUrl>
#include <QTextEdit>
#include <QTimer>
#include <QCryptographicHash>
#include <QThread>

ChatGpt::ChatGpt(QWidget* parent)
	: BasePlugin(parent)
{
	QVBoxLayout* layout = new QVBoxLayout();
	_view->setPage(_page);

	_view->load(_url);
	_view->setZoomFactor(1.2);
	layout->addWidget(_view);
	this->setLayout(layout);
	layout->setContentsMargins(0, 0, 0, 0);
	_timer->setInterval(100);
	ChatGpt::connect(_timer, &QTimer::timeout, this, &ChatGpt::timeouted);
	connect(_view, &QWebEngineView::loadFinished, this, &ChatGpt::loadFinish);
	this->setWindowIcon(this->getIcon());
}

ChatGpt::~ChatGpt()
{
};

void ChatGpt::request(const QString& text, bool running)
{
	qDebug() << text;
	QString jsCode;
	if (running)
	{
		jsCode = QString(R"(
			(function() {
				var button = document.evaluate('/html/body/div[1]/div[1]/div[2]/main/div[1]/div[2]/div[1]/div/form/div[2]/div[2]/div/div/button',
					document, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;
				if(button){
					console.log( button);
					button.click();
				}
			})();
		)");
		_view->page()->runJavaScript(jsCode);
		QThread::sleep(0.1);
		jsCode = QString(R"(
			(function() {
				var button = document.evaluate('/html/body/div[1]/div[1]/div[1]/div/div/div/div/nav/div[2]/div[1]/div/a/div[3]/span',
					document, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;
				if(button){
					button.click();
				}
			})();
		)");
		_view->page()->runJavaScript(jsCode);
		QThread::sleep(0.5);
	}
	jsCode = QString(R"(
				(function() {
					var textarea = document.evaluate('/html/body/div[1]/div[1]/div[2]/main/div[1]/div[2]/div[1]/div/form/div[2]/div[2]/div/div/div[2]/textarea', document, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;
					if (textarea) {
						textarea.value = "%1";
						var event = new Event('input', { bubbles: true });
						textarea.dispatchEvent(event);
						event = new Event('change', { bubbles: true });
						textarea.dispatchEvent(event);
					}
					else {
						console.log("textarea not found");
					}
				})();
			)").arg(LFunc::escapeString(text, true));
	qDebug() << jsCode;
	_view->page()->runJavaScript(jsCode);
	QThread::sleep(0.1);
	jsCode = QString(R"(
		(function() {
			var button = document.evaluate('/html/body/div[1]/div[1]/div[2]/main/div[1]/div[2]/div[1]/div/form/div[2]/div[2]/div/div/button', document, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;
			if (button && "%2" == "true") {
				button.click();
			}
		})();
	)").arg(running ? "true" : "false");
	_view->page()->runJavaScript(jsCode);
}

void ChatGpt::timeouted()
{
	QString jsCode = R"(
			(function() {
				var xpath = "/html/body/div[1]/div[1]/div[2]/main/div[1]/div[1]/div/div/div/div/div[3]/div/div/div[2]/div/div[1]";
				var result = document.evaluate(xpath, document, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null);
				var targetElement = result.singleNodeValue; // Use a different variable name

				if (targetElement) {
					targetElement.innerHTML
				} else {
					console.log("Element not found");
				}
			})();
		)";
	_view->page()->runJavaScript(jsCode, [&](const QVariant& result)
		{
			QString text = result.toString();
			if (text == "Node not found") {
				return;
			}
			QString hash = QCryptographicHash::hash(text.toUtf8(), QCryptographicHash::Sha1).toHex();
			if (hash != _lastHash)
			{
				_lastHash = hash;
				this->setReply(text);
			}
		});
}

void ChatGpt::setReplyRunning(const bool running)
{
	if (running)
	{
		_timer->start();
	}
	else
	{
		_timer->stop();
	}
}

void ChatGpt::load()
{
	_view->load(_url);
	_view->reload();
}