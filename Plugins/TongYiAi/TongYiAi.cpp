#include "TongYiAi.h"
#include <QVBoxLayout>
#include <LCore>
TongYiAi::TongYiAi(QWidget* parent)
{
	QVBoxLayout* layout = new QVBoxLayout();
	_view->setPage(_page);

	_view->load(_url);
	_view->setZoomFactor(1.2);
	layout->addWidget(_view);
	layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(layout);
	_timer->setInterval(100);
	connect(_timer, &QTimer::timeout, this, &TongYiAi::timeouted);
	connect(_view, &QWebEngineView::loadFinished, this, &TongYiAi::loadFinish);
	this->setWindowIcon(this->getIcon());
}

void TongYiAi::request(const QString& text, bool running)
{
	QString jsCode;
	if (running)
	{
		jsCode = QString(R"(

			var button = document.evaluate('/html/body/div[1]/div/div/div[4]/div[4]/div[3]/div[2]', document, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;
		    if (button) {
		        button.click();
		    } else {
		        console.log("Button not found");
		    }
		)");
		_view->page()->runJavaScript(jsCode);
		QThread::sleep(0.1);
		jsCode = QString(R"(
			(function() {
				var newButton = document.evaluate('/html/body/div[1]/div[1]/div/div[3]/button', document, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;
				if(newButton){
					newButton.click();
				}
			})();
		)");
		_view->page()->runJavaScript(jsCode);
		QThread::sleep(0.3);
	}
	jsCode = QString(R"(
				(function() {
				    var textarea = document.querySelector('.chatInput--eJzBH8LP .chatTextarea--tMt0p9_a .textareaWrap--x_vNvkR2 textarea');

				    if (textarea) {
						function setNativeValue(element, value) {
						  const valueSetter = Object.getOwnPropertyDescriptor(element, 'value').set;
						  const prototype = Object.getPrototypeOf(element);
						  const prototypeValueSetter = Object.getOwnPropertyDescriptor(prototype, 'value').set;

						  if (valueSetter && valueSetter !== prototypeValueSetter) {
  							prototypeValueSetter.call(element, value);
						  } else {
						    valueSetter.call(element, value);
						  }
						}
						setNativeValue(textarea,"%1");
						textarea.dispatchEvent(new Event('input', { bubbles: true }));
				    } else {
				        console.log("Textarea not found");
				    }
				})();
			)").arg(LFunc::escapeString(text, true));

	_view->page()->runJavaScript(jsCode);
	if (running)
	{
		QThread::sleep(0.3);
		jsCode = QString(R"(
			var button = document.evaluate('/html/body/div[1]/div/div/div[4]/div[4]/div[3]/div[2]', document, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;
		    if (button) {
		        button.click();
		    } else {
		        console.log("Button not found");
		    }
		)");
		_view->page()->runJavaScript(jsCode);
	}
}

void TongYiAi::setReplyRunning(bool running)
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

void TongYiAi::setReply(const QString& text)
{
	BasePlugin::setReply(text);
}

void TongYiAi::load()
{
	_view->load(_url);
	_view->reload();
}

void TongYiAi::timeouted()
{
	QString jsCode = R"(
		// 找到包含回答问题 HTML 内容的父级元素
		var answerContainer = document.querySelector('.answerItem--U4_Uv3iw');

		// 找到回答问题的 HTML 内容元素
		var answerContentElement = answerContainer.querySelector('.stream--brXJQIKe');

		// 获取回答问题的 HTML 内容
		answerContentElement.innerHTML;

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