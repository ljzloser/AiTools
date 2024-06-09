#include "AiTools.h"
#include <QClipboard>

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
	if (!showTime) return;
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
	connect(this, &LWidget::systemSettingsChanged, tool, &AiTools::loadConfig);
	tool->loadConfig();
	this->setWindowIcon(QIcon(":/AiTools/icon/AI.png"));
	this->setWindowFlags(this->windowFlags() | Qt::SubWindow | Qt::WindowStaysOnTopHint);
}

void Widget::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::ActivationChange)
		if (!isActiveWindow() && focusHide)
			this->hide();
	QWidget::changeEvent(event);
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
	if (_loginDialog)
		_loginDialog->deleteLater();
}

void AiTools::initUi()
{
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);

	QHBoxLayout* oneRow = new QHBoxLayout();
	_openButton->setIcon(QIcon(":/AiTools/icon/link.png"));
	_openButton->setFixedSize(_openButton->iconSize());
	_openButton->setToolTip("打开链接");
	oneRow->addWidget(_openButton);
	oneRow->addWidget(_comboBox);

	QHBoxLayout* twoRow = new QHBoxLayout();
	_clearButton->setIcon(QIcon(":/AiTools/icon/clear.png"));
	_clearButton->setFixedSize(_clearButton->iconSize());
	_clearButton->setToolTip("清空");
	_sendAction->setIcon(QIcon(":/AiTools/icon/send.png"));
	_inputLineEdit->addAction(_sendAction, QLineEdit::TrailingPosition);
	twoRow->addWidget(_clearButton);
	twoRow->addWidget(_inputLineEdit);

	QHBoxLayout* threeRow = new QHBoxLayout();
	_copyButton->setIcon(QIcon(":/AiTools/icon/copy.png"));
	_copyButton->setFixedSize(_copyButton->iconSize());
	_copyButton->setToolTip("复制");
	QSpacerItem* item = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
	threeRow->addWidget(_copyButton);
	threeRow->addItem(item);

	_textEdit->setReadOnly(true);
	_textEdit->setLineNumberAreaVisible(true);

	_webDialog->setFixedHeight(1);

	layout->addLayout(oneRow);
	layout->addLayout(twoRow);
	layout->addLayout(threeRow);
	layout->addWidget(_textEdit);
	layout->addWidget(_webDialog);
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
	connect(_webDialog, &WebDialog::reply, this, &AiTools::reply);
	connect(_loginAction, &QAction::triggered, this, &AiTools::openLoginDialog);
	connect(_comboBox->lineEdit(), &QLineEdit::returnPressed, this, &AiTools::sendMessage);
	connect(_copyButton, &QPushButton::clicked, [=]() {QApplication::clipboard()->setText(_textEdit->toPlainText()); });
	connect(_settingAction, &QAction::triggered, this, &AiTools::openSettingDialog);
}

void AiTools::loadConfig()
{
	if (_parent == nullptr)
		_parent = static_cast<Widget*>(this->parent());
	LJsonConfig config(QApplication::applicationDirPath() + "/config.json");
	QJsonObject obj = config.readJson().object();
	_showHotkey->setShortcut(obj.value("keySequence").toString(), true);
	_autoFill = obj.value("autoFill").toBool();
	bool isdark = false;
	switch (obj.value("theme").toInt())
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
	if ((isdark != (_theme == 1)) || _theme == -1)
	{
		QString filename = isdark ? ":/qdarkstyle/dark/darkstyle.qss" : ":/qdarkstyle/light/lightstyle.qss";
		QFile file(filename);
		file.open(QFile::ReadOnly);
		if (file.isOpen())
		{
			qobject_cast<QApplication*>(QCoreApplication::instance())->setStyleSheet(QLatin1String(file.readAll()));
			_theme = isdark;
			QColor color = _theme == 0 ? QColor("#fafafa") : QColor("#19232D");
			LWidget::Info info = _parent->info();
			info.backgroundColor = color;
			info.splitLineColor = color;
			_parent->setInfo(info);
		}
	}
	_parent->setWindowOpacity(obj.value("transparent").toDouble());
	_showHotkey->setShortcut(QKeySequence::fromString(obj.value("keySequence").toString()), true);
	_parent->focusHide = obj.value("focusHide").toBool();
	_pointMode = obj.value("pointMode").toInt();
	TitleBar* titleBar = static_cast<TitleBar*>(_parent->getTitleBar());
	titleBar->showTime = obj.value("showTime").toBool();
	LJsonConfig prompt(QApplication::applicationDirPath() + "/prompt.json");
	QJsonDocument doc = prompt.readJson();
	if (doc.isNull())
		prompt.init(QJsonDocument::fromJson("{}"));
	obj = prompt.readJson().object();
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

void AiTools::reply(const QString& text) const
{
	QTextDocument doc;
	doc.setHtml(text);
	_textEdit->setPlainText(doc.toPlainText());
	_textEdit->moveCursor(QTextCursor::End);
}

void AiTools::onHotkeyPressed() const
{
	if (this->parentWidget() == nullptr)
		return;
	if (this->isVisible())
	{
		this->parentWidget()->hide();
		_webDialog->setReplyRunning(false);
	}
	else
	{
		QRect rect = this->parentWidget()->geometry();
		auto pos = QCursor::pos();
		switch (_pointMode)
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
		_inputLineEdit->setText(_autoFill ? QApplication::clipboard()->text() : "");
		_inputLineEdit->setFocus();
		_inputLineEdit->setCursorPosition(_inputLineEdit->text().length());
		_webDialog->setReplyRunning(true);
	}
}

void AiTools::systemTrayIconActivated(QSystemTrayIcon::ActivationReason reason) const
{
	switch (reason)
	{
	case QSystemTrayIcon::DoubleClick:
		this->onHotkeyPressed();
		break;
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
	_webDialog->request(_inputLineEdit->text() + _comboBox->currentData().toString());
}

void AiTools::openLoginDialog()
{
	// 打开指定网页
	//QDesktopServices::openUrl(QUrl("https://chatglm.cn/main/alltoolsdetail"));
	if (_loginDialog != nullptr)
		_loginDialog->deleteLater();
	_loginDialog = new WebDialog();
	_loginDialog->setWindowFlag(Qt::Tool);
	_loginDialog->resize(1600, 900);
	_loginDialog->show();
	connect(_loginDialog, &WebDialog::closed, _webDialog, &WebDialog::load);
}

void AiTools::openSettingDialog()
{
	ConfigDialog dialog;
	_parent->hide();
	_showHotkey->setShortcut(_showHotkey->shortcut(), false); // 这一句好像没有用
	connect(&dialog, &ConfigDialog::saved, this, &AiTools::loadConfig);
	dialog.exec();
}