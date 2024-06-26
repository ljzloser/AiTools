﻿#include "ZhiPuAi.h"
#include <QVBoxLayout>
#include <QUrl>
#include <QTextEdit>
#include <QTimer>
#include <QCryptographicHash>
#include <QThread>

ZhiPuAi::ZhiPuAi(QWidget* parent)
	: BasePlugin(parent)
{
	QVBoxLayout* layout = new QVBoxLayout();
	_view->setPage(_page);

	_view->load(_url);
	_view->setZoomFactor(1.2);
	layout->addWidget(_view);
	layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(layout);
	_timer->setInterval(100);
	ZhiPuAi::connect(_timer, &QTimer::timeout, this, &ZhiPuAi::timeouted);
	connect(_view, &QWebEngineView::loadFinished, this, &ZhiPuAi::loadFinish);
	this->setWindowIcon(this->getIcon());
}

ZhiPuAi::~ZhiPuAi()
{
};

void ZhiPuAi::request(const QString& text, bool running)
{
	QString jsCode;
	if (running)
	{
		jsCode = QString(R"(
			(function() {
				var button = document.evaluate('/html/body/div[1]/div/section/main/div/div[2]/div[2]/div/div/div[2]/div[3]/div[1]/div[1]/div[4]/div/div/div/div[3]/img', document, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;
				if(button){
					button.click();
				}
			})();
		)");
		_view->page()->runJavaScript(jsCode);
		QThread::sleep(0.1);
		jsCode = QString(R"(
			(function() {
				var newButton = document.evaluate('/html/body/div[1]/div/section/main/div/div[1]/div/div[1]/div[2]/div', document, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;
				if(newButton){
					newButton.click();
				}
			})();
		)");
		_view->page()->runJavaScript(jsCode);
		QThread::sleep(0.5);
	}
	jsCode = QString(R"(
				(function() {
					var textarea = document.evaluate('/html/body/div[1]/div/section/main/div/div[2]/div[2]/div/div/div[2]/div[3]/div[1]/div[1]/div[4]/div/div/div/div[2]/textarea', document, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;
					var button = document.evaluate('/html/body/div[1]/div/section/main/div/div[2]/div[2]/div/div/div[2]/div[3]/div[1]/div[1]/div[4]/div/div/div/div[3]/img', document, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;

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
					if (button && "%2" == "true") {
						button.click();
					}
				})();
			)").arg(LFunc::escapeString(text, true), running ? "true" : "false");

	_view->page()->runJavaScript(jsCode);
}

void ZhiPuAi::timeouted()
{
	QString jsCode = R"(
			var node = document.querySelector('.answer-content-wrap');
			if (node) {
				node.outerHTML;
			} else {
				"Node not found";
			}
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

void ZhiPuAi::setReplyRunning(const bool running)
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

void ZhiPuAi::load()
{
	_view->load(_url);
	_view->reload();
}