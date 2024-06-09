#include "AiTools.h"
#include <QDesktopServices>
#include "LWidget"
AiTools::AiTools(QWidget* parent)
	: QWidget(parent)
{
	_parent = static_cast<LWidget*>(parent);
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	QHBoxLayout* oneRow = new QHBoxLayout();
	_openButton->setIcon(QIcon(":/AiTools/icon/link.png"));
	_openButton->setFixedSize(_openButton->iconSize());
	_openButton->setToolTip("打开链接");
	oneRow->addWidget(_openButton);
	oneRow->addWidget(_comboBox);

	_sendAction->setIcon(QIcon(":/AiTools/icon/send.png"));
	_inputLineEdit->addAction(_sendAction, QLineEdit::TrailingPosition);
	QHBoxLayout* threeRow = new QHBoxLayout();
	_copyButton->setIcon(QIcon(":/AiTools/icon/copy.png"));
	_copyButton->setFixedSize(_copyButton->iconSize());
	_copyButton->setToolTip("复制");
	QSpacerItem* item = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
	threeRow->addWidget(_copyButton);
	threeRow->addItem(item);
	_textEdit->setReadOnly(true);
	_textEdit->setLineNumberAreaVisible(true);
	layout->addLayout(oneRow);
	layout->addWidget(_inputLineEdit);
	layout->addLayout(threeRow);
	layout->addWidget(_textEdit);
	layout->addWidget(_webDialog);

	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
	_webDialog->setFixedHeight(1);
	this->setLayout(layout);

	_showHotkey->setShortcut(QKeySequence(), true);

	_bootStrapAction->setCheckable(true);
	_trayIcon->setIcon(QIcon(":/AiTools/icon/AI.png"));
	_trayMenu->addAction(_loginAction);
	_trayMenu->addAction(_settingAction);
	//_trayMenu->addAction(_bootStrapAction);
	_trayMenu->addAction(_quitAction);
	_trayIcon->setContextMenu(_trayMenu);
	_trayIcon->show();
	LJsonConfig _config(QApplication::applicationDirPath() + "/config.json");
	_bootStrapAction->setChecked(_config.read("autoRun").toBool());
	this->initConnect();
}

AiTools::~AiTools()
{
	if (_loginDialog)
		_loginDialog->deleteLater();
}

void AiTools::initConnect()
{
	connect(_showHotkey, SIGNAL(activated()), this, SLOT(onHotkeyPressed()));
	connect(_trayIcon, &QSystemTrayIcon::activated, this, &AiTools::systemTrayIconActivated);
	connect(_quitAction, &QAction::triggered, this, &QApplication::quit);
	connect(_bootStrapAction, &QAction::triggered, this, [=](bool checked)
		{
			LFunc::autoRun(checked, "AiTools");
			LJsonConfig config(QApplication::applicationDirPath() + "/config.json");
			config.write("autoRun", checked);
		});
	connect(_sendAction, &QAction::triggered, this, &AiTools::sendMessage);
	connect(_inputLineEdit, &QLineEdit::returnPressed, this, &AiTools::sendMessage);
	connect(_inputLineEdit, &QLineEdit::textChanged, this, &QLineEdit::setToolTip);
	connect(_openButton, &QPushButton::clicked, this, &AiTools::openButtonClicked);
	connect(_webDialog, &WebDialog::reply, this, &AiTools::reply);
	connect(_loginAction, &QAction::triggered, this, [=]()
		{
			if (_loginDialog != nullptr)
				_loginDialog->deleteLater();
			_loginDialog = new WebDialog();
			_loginDialog->setWindowFlag(Qt::Tool);
			_loginDialog->show();
			connect(_loginDialog, &WebDialog::closed, _webDialog, &WebDialog::load);
		});
	connect(_comboBox->lineEdit(), &QLineEdit::returnPressed, this, &AiTools::sendMessage);
	connect(_copyButton, &QPushButton::clicked, [=]() {QApplication::clipboard()->setText(_textEdit->toPlainText()); });
	connect(_settingAction, &QAction::triggered, this, [=]()
		{
			ConfigDialog dialog;
			_parent->hide();
			_showHotkey->setShortcut(_showHotkey->shortcut(), false);
			connect(&dialog, &ConfigDialog::saved, this, &AiTools::loadConfig);
			dialog.exec();
		});
}

void AiTools::loadConfig()
{
	_parent = (LWidget*)this->parent();
	LJsonConfig config(QApplication::applicationDirPath() + "/config.json");
	QJsonObject obj = config.readJson().object();
	_showHotkey->setShortcut(obj.value("keySequence").toString(), true);
	_autoFill = obj.value("autoFill").toBool();
	bool isdark = false;
	switch (obj.value("theme").toInt())
	{
	case 0:
	{
		isdark = false;
		break;
	}
	case 1:
	{
		isdark = true;
		break;
	}
	case 2:
	{
		isdark = LFunc::IsSystemDarkModeActive();
		break;
	}
	default:
		break;
	}
	if ((isdark != (theme == 1)) || theme == -1)
	{
		QString filename = isdark ? ":/qdarkstyle/dark/darkstyle.qss" : ":/qdarkstyle/light/lightstyle.qss";
		QFile file(filename);
		file.open(QFile::ReadOnly);
		if (file.isOpen())
		{
			QString styleSheet = QLatin1String(file.readAll());
			qobject_cast<QApplication*>(QCoreApplication::instance())->setStyleSheet(styleSheet);
		}
		theme = isdark;
		if (theme == 0)
		{
			auto info = _parent->info();
			info.backgroundColor = QColor("#fafafa");
			info.splitLineColor = QColor("#fafafa");
			_parent->setInfo(info);
		}
		else
		{
			auto info = _parent->info();
			info.backgroundColor = QColor("#19232D");
			info.splitLineColor = QColor("#19232D");
			_parent->setInfo(info);
		}
	}
	_parent->setWindowOpacity(obj.value("transparent").toDouble());
	_showHotkey->setShortcut(QKeySequence::fromString(obj.value("keySequence").toString()), true);
	LJsonConfig prompt(QApplication::applicationDirPath() + "/prompt.json");
	QJsonDocument doc = prompt.readJson();
	if (doc.isNull())
		prompt.init(QJsonDocument::fromJson("{}"));
	doc = prompt.readJson();
	obj = doc.object();
	if (!obj.isEmpty())
	{
		_comboBox->clear();
		for (auto it = obj.begin(); it != obj.end(); it++)
		{
			_comboBox->addItem(it.key() + QString("(%1)").arg(it.value().toString()), it.value().toString());
		}
	}
	_comboBox->setCurrentIndex(-1);
}

void AiTools::reply(const QString& text)
{
	QTextDocument doc;
	doc.setHtml(text);
	_textEdit->setPlainText(doc.toPlainText());
	_textEdit->moveCursor(QTextCursor::End);
}

void AiTools::onHotkeyPressed()
{
	if (this->parentWidget() == nullptr)
	{
		return;
	}
	if (this->isVisible())
	{
		this->parentWidget()->hide();
		_webDialog->setReplyRunning(false);
	}
	else
	{
		QRect rect = this->parentWidget()->geometry();
		rect.moveCenter(QCursor::pos());
		this->parentWidget()->setGeometry(rect);
		this->parentWidget()->show();
		_inputLineEdit->setText(_autoFill ? QApplication::clipboard()->text() : "");
		_inputLineEdit->setFocus();
		_inputLineEdit->setCursorPosition(_inputLineEdit->text().length());
		_webDialog->setReplyRunning(true);
	}
}

void AiTools::systemTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
	case QSystemTrayIcon::DoubleClick:
		this->onHotkeyPressed();
		break;
	default:
		break;
	}
}

void AiTools::sendMessage()
{
	_webDialog->request(_inputLineEdit->text() + _comboBox->currentData().toString());
}

void AiTools::openButtonClicked()
{
	// 打开指定网页
	QDesktopServices::openUrl(QUrl("https://chatglm.cn/main/alltoolsdetail"));
}