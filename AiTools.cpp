#include "AiTools.h"
#include <QClipboard>
#include <Windows.h>

static UINT simulateCtrlC() {
	INPUT inputs[4];

	// 按下 Ctrl
	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_CONTROL;
	inputs[0].ki.dwFlags = 0;

	// 按下 C
	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = 'C';
	inputs[1].ki.dwFlags = 0;

	// 释放 C
	inputs[2].type = INPUT_KEYBOARD;
	inputs[2].ki.wVk = 'C';
	inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

	// 释放 Ctrl
	inputs[3].type = INPUT_KEYBOARD;
	inputs[3].ki.wVk = VK_CONTROL;
	inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

	return SendInput(4, inputs, sizeof(INPUT));
}

TitleBar::TitleBar(QWidget* parent)
	: LTitleBar(parent)
{
	this->getCloseButton()->hide();
	this->getMaxButton()->hide();
	this->getMinButton()->hide();
	this->setTitleIcon(QIcon());
	this->setTitleText("");
}

void TitleBar::paintEvent(QPaintEvent* event)
{
	LTitleBar::paintEvent(event);
	if (!Config::instance().showTime) return;
	QPainter painter(this);
	QFont font = this->font();
	font.setBold(true);
	painter.setFont(font);
	const QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
	QRect rect = this->rect();
	rect.setHeight(rect.height() - 8);
	painter.drawText(rect, Qt::AlignCenter, dateTime);
}

Widget::Widget(LBaseTitleBar* titleBar, QWidget* mainWidget, QWidget* parent)
	: LWidget(titleBar, mainWidget, parent)
{
	AiTools* tool = static_cast<AiTools*>(mainWidget);
	connect(this, &LWidget::systemSettingsChanged, [tool]() { tool->loadConfig(false); });
	tool->loadConfig(true);
	this->setWindowIcon(QIcon(":/AiTools/icon/AI.png"));
	this->setWindowFlags(this->windowFlags() | Qt::SubWindow | Qt::WindowStaysOnTopHint);
	this->resize(Config::instance().width, Config::instance().height);
}

void Widget::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::ActivationChange)
		if (!isActiveWindow() && Config::instance().focusHide)
			this->hide();
	if (event->type() == QEvent::WindowStateChange)
		if (this->isMinimized())
		{
			this->showNormal();
			this->hide();
		}
	QWidget::changeEvent(event);
}

void Widget::resizeEvent(QResizeEvent* event)
{
	LWidget::resizeEvent(event);
	Config::instance().width = this->width();
	Config::instance().height = this->height();
}

AiTools::AiTools(QWidget* parent)
	: QWidget(parent), _parent(static_cast<Widget*>(parent))
{
	this->initUi();
	this->initTratIcon();
	this->initConnect();
}

AiTools::~AiTools()
{
	if (_loginDialog) _loginDialog->deleteLater();
	if (_configDialog) _configDialog->deleteLater();
}

void AiTools::initUi()
{
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);

	QHBoxLayout* oneRow = new QHBoxLayout();
	_settingButton->setIcon(QIcon(":/AiTools/icon/set.png"));
	_settingButton->setFixedSize(_settingButton->iconSize());
	_settingButton->setToolTip("设置");
	_promptComboBox->lineEdit()->setPlaceholderText("请输入提示词。");
	_updatePromptAction->setIcon(QIcon(":/AiTools/icon/update.png"));
	_promptComboBox->lineEdit()->addAction(_updatePromptAction, QLineEdit::TrailingPosition);
	oneRow->addWidget(_settingButton);
	oneRow->addWidget(_promptComboBox);

	QHBoxLayout* twoRow = new QHBoxLayout();
	_clearButton->setIcon(QIcon(":/AiTools/icon/clear.png"));
	_clearButton->setFixedSize(_clearButton->iconSize());
	_clearButton->setToolTip("清空");
	_sendAction->setIcon(QIcon(":/AiTools/icon/send.png"));
	_sendAction->setToolTip("发送");
	_inputLineEdit->addAction(_sendAction, QLineEdit::TrailingPosition);
	_inputLineEdit->setPlaceholderText("请输入提问的内容，按下回车发送。");
	twoRow->addWidget(_clearButton);
	twoRow->addWidget(_inputLineEdit);

	QHBoxLayout* threeRow = new QHBoxLayout();
	_copyButton->setIcon(QIcon(":/AiTools/icon/copy.png"));
	_copyButton->setFixedSize(_copyButton->iconSize());
	_copyButton->setToolTip("复制");
	_openButton->setIcon(QIcon(":/AiTools/icon/link.png"));
	_openButton->setFixedSize(_openButton->iconSize());
	_openButton->setToolTip("打开链接");
	_debugButton->setIcon(QIcon(":/AiTools/icon/Debug.png"));
	_debugButton->setFixedSize(_debugButton->iconSize());
	_debugButton->setToolTip("打开交互窗口");
	QSpacerItem* item = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
	threeRow->addWidget(_copyButton);
	threeRow->addWidget(_openButton);
	threeRow->addWidget(_debugButton);
	threeRow->addItem(item);

	_textEdit->setReadOnly(true);
	_textEdit->setLineNumberAreaVisible(true);

	layout->addLayout(oneRow);
	layout->addLayout(twoRow);
	layout->addLayout(threeRow);
	layout->addWidget(_textEdit);
	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));

	this->setLayout(layout);
}

void AiTools::initTratIcon()
{
	/*_bootStrapAction->setCheckable(true);
	_trayMenu->addAction(_bootStrapAction);
	LJsonConfig _config(QApplication::applicationDirPath() + "/config.json");
	_bootStrapAction->setChecked(_config.read("autoRun").toBool());*/
	_loginAction->setIcon(QIcon(":/AiTools/icon/login.png"));
	_trayMenu->QWidget::addAction(_loginAction);
	_settingAction->setIcon(QIcon(":/AiTools/icon/set.png"));
	_trayMenu->QWidget::addAction(_settingAction);
	_quitAction->setIcon(QIcon(":/AiTools/icon/exit.png"));
	_trayMenu->QWidget::addAction(_quitAction);
	_trayIcon->setIcon(QIcon(":/AiTools/icon/AI.png"));
	_trayIcon->setContextMenu(_trayMenu);
	_trayIcon->show();
}

void AiTools::initConnect()
{
	connect(_showHotkey, SIGNAL(activated()), this, SLOT(onHotkeyPressed()));
	connect(_trayIcon, &QSystemTrayIcon::activated, this, &AiTools::systemTrayIconActivated);
	connect(_quitAction, &QAction::triggered, this, &QApplication::quit);
	/*connect(_inputLineEdit, &QLineEdit::textChanged, this, &QLineEdit::setToolTip);
	connect(_bootStrapAction, &QAction::triggered, this, [=](bool checked)
		{
			LFunc::autoRun(checked, "AiTools");
			LJsonConfig config(QApplication::applicationDirPath() + "/config.json");
			config.write("autoRun", checked);
		});*/
	connect(_sendAction, &QAction::triggered, this, &AiTools::sendMessage);
	connect(_inputLineEdit, &QLineEdit::returnPressed, this, &AiTools::sendMessage);
	connect(_clearButton, &QPushButton::clicked, _inputLineEdit, &QLineEdit::clear);
	connect(_clearButton, &QPushButton::clicked, _textEdit, &LTextEdit::clear);
	connect(_openButton, &QPushButton::clicked, this, &AiTools::openLoginDialog);
	connect(_loginAction, &QAction::triggered, this, &AiTools::openLoginDialog);
	connect(_promptComboBox->lineEdit(), &QLineEdit::returnPressed, this, &AiTools::sendMessage);
	connect(_copyButton, &QPushButton::clicked, [=]() {QApplication::clipboard()->setText(_textEdit->toPlainText()); });
	connect(_settingAction, &QAction::triggered, this, &AiTools::openSettingDialog);
	connect(_settingButton, &QPushButton::clicked, this, &AiTools::openSettingDialog);
	connect(_updatePromptAction, &QAction::triggered, [=]()
		{
			const QString path = "file:///" + QApplication::applicationDirPath() + "/prompt.json";
			QDesktopServices::openUrl(QUrl(path));
			_parent->hide();
		});
	connect(_debugButton, &QPushButton::clicked, this, &AiTools::debugButtonClicked);
}

void AiTools::loadConfig(bool init)
{
	if (_webDialog != nullptr || _webDialog->objectName().isEmpty())
	{
		QString name = _webDialog->objectName();
		if (name != Config::instance().pluginInfo.fileName)
		{
			this->layout()->removeWidget(_webDialog);
			_webDialog->deleteLater();
			_webDialog = nullptr;
		}
	}
	if (_webDialog == nullptr)
	{
		_webDialog = Config::instance().plugin();
		if (_webDialog)
		{
			_webDialog->setMaximumHeight(1);
			this->layout()->addWidget(_webDialog);
			connect(_webDialog, &BasePlugin::reply, this, &AiTools::reply);
		}
	}
	_textEdit->setPlaceholderText(QString("%1返回内容会在此展示，点击复制即可复制到剪切板。").arg(Config::instance().pluginInfo.name));

	if (_parent == nullptr)
		_parent = static_cast<Widget*>(this->parent());
	bool isdark = false;
	switch (Config::instance().theme)
	{
	case 0:
		isdark = false;
		break;
	case 1:
		isdark = true;
		break;
	case 2:
		isdark = LFunc::IsSystemDarkModeActive();
		break;
	default:
		break;
	}
	if (_isdark != isdark || init)
	{
		QString filename = isdark ? ":/qdarkstyle/dark/darkstyle.qss" : ":/qdarkstyle/light/lightstyle.qss";
		QFile file(filename);
		file.open(QFile::ReadOnly);
		if (file.isOpen())
		{
			qobject_cast<QApplication*>(QCoreApplication::instance())->setStyleSheet(QLatin1String(file.readAll()));
			_isdark = isdark;
			QColor color = !_isdark ? QColor("#fafafa") : QColor("#19232D");
			LWidget::Info info = _parent->info();
			info.backgroundColor = color;
			info.splitLineColor = color;
			_parent->setInfo(info);
		}
	}
	_parent->setWindowOpacity(Config::instance().transparent);
	_showHotkey->setShortcut(Config::instance().keySequence, true);

	LJsonConfig prompt(QApplication::applicationDirPath() + "/prompt.json");
	QJsonDocument doc = prompt.readJson();
	if (doc.isNull()) // 初始化防止有人吧prompt.json删了
		prompt.init(QJsonDocument::fromJson("{}"));
	auto obj = prompt.readJson().object();
	if (!obj.isEmpty())
	{
		_promptComboBox->clear();
		for (auto it = obj.begin(); it != obj.end(); ++it)
		{
			_promptComboBox->addItem(it.key() + QString("(%1)").arg(it.value().toString()), it.value().toString());
		}
	}
	_promptComboBox->setCurrentIndex(-1);
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
		return;
	if (this->isVisible())
	{
		this->parentWidget()->hide();
		if (_webDialog)
			_webDialog->setReplyRunning(false);
	}
	else
	{
		this->loadConfig();
		if (Config::instance().autoCopy)
			simulateCtrlC();
		if (!Config::instance().lastPrompt)
			_promptComboBox->setCurrentIndex(-1);
		QRect rect = this->parentWidget()->geometry();
		auto pos = QCursor::pos();
		switch (Config::instance().pointMode)
		{
		case 0:
			rect.moveCenter(pos);
			break;
		case 1:
			rect.moveTopLeft(pos);
			break;
		case 2:
			rect.moveBottomLeft(pos);
			break;
		case 3:
			rect.moveTopRight(pos);
			break;
		case 4:
			rect.moveBottomRight(pos);
			break;
		case 5:
		{
			rect.moveCenter(pos);
			rect.moveTop(pos.y());
			break;
		}
		case 6:
		{
			rect.moveCenter(pos);
			rect.moveBottom(pos.y());
			break;
		}
		case 7:
		{
			rect.moveCenter(pos);
			rect.moveLeft(pos.x());
			break;
		}
		case 8:
		{
			rect.moveCenter(pos);
			rect.moveRight(pos.x());
			break;
		}
		default:
			break;
		}
		this->parentWidget()->setGeometry(rect);
		this->parentWidget()->show();
		_parent->activateWindow();
		_inputLineEdit->setText(Config::instance().autoFill ? QApplication::clipboard()->text() : "");
		switch (Config::instance().focusPoint)
		{
		case 0:
			_promptComboBox->lineEdit()->setFocus();
			break;
		case 1:
			_inputLineEdit->setFocus();
			_inputLineEdit->setCursorPosition(_inputLineEdit->text().length());
			break;
		default:
			break;
		}
		if (_webDialog)
			_webDialog->setReplyRunning(true);
	}
}

void AiTools::systemTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
	case QSystemTrayIcon::DoubleClick:
	{
		this->onHotkeyPressed();
		QRect rect = this->parentWidget()->geometry();
		QPoint center = QGuiApplication::primaryScreen()->availableGeometry().center();
		rect.moveCenter(center);
		this->parentWidget()->setGeometry(rect);
		break;
	}
	case QSystemTrayIcon::Unknown:
		break;
	case QSystemTrayIcon::Context:
		break;
	case QSystemTrayIcon::Trigger:
		break;
	case QSystemTrayIcon::MiddleClick:
		break;
	default:
		break;
	}
}

void AiTools::sendMessage() const
{
	const QString text = _inputLineEdit->text();
	if (text.isEmpty())
		return;
	QString prompt = _promptComboBox->currentData().toString();
	const QString comboBoxText = _promptComboBox->lineEdit()->text();
	prompt = comboBoxText == QString("%1(%2)").arg(_promptComboBox->currentText(), prompt) ? prompt : comboBoxText;
	const QString question = Config::instance().promptPoint == 0 ? (prompt + " " + text) : (text + " " + prompt);
	if (_webDialog)
		_webDialog->request(question);
}

void AiTools::openLoginDialog()
{
	// 打开指定网页
	//QDesktopServices::openUrl(QUrl("https://chatglm.cn/main/alltoolsdetail"));
	if (_loginDialog != nullptr) _loginDialog->deleteLater();
	_loginDialog = Config::instance().plugin();
	if (_loginDialog == nullptr) return;
	_loginDialog->setWindowFlag(Qt::Tool);
	_loginDialog->resize(1600, 900);
	_loginDialog->show();
	connect(_loginDialog, &BasePlugin::closed, _webDialog, &BasePlugin::load);
}

void AiTools::openSettingDialog()
{
	if (_configDialog) _configDialog->deleteLater();
	_configDialog = new ConfigDialog();
	_parent->hide();
	_showHotkey->setShortcut(_showHotkey->shortcut(), false); // 这一句好像没有用
	connect(_configDialog, &ConfigDialog::saved, this, &AiTools::loadConfig);
	_configDialog->show();
}

void AiTools::debugButtonClicked()
{
	int height = _webDialog->maximumHeight();
	if (height < 10)
	{
		_webDialog->setMaximumHeight(QWIDGETSIZE_MAX);
		_webDialog->resize(_webDialog->width(), 200);
	}
	else
	{
		_webDialog->setMaximumHeight(1);
	}
}