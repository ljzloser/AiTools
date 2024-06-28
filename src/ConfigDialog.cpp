#include "ConfigDialog.h"
#include <QPluginLoader>
#include <QDesktopServices>
#include "Update.h"

void Config::Field::setValue(QJsonObject& obj)
{
	if (obj.contains(name))
		value = obj.value(name).toVariant();
	else
		obj[name] = value.toJsonValue();
}

Config& Config::instance()
{
	static Config instance;

	return instance;
}

QJsonObject Config::toJson() const
{
#define ConfigPair(Key,type,func) {Key.name , Key.value.value<type>()func}
	return QJsonObject{
		ConfigPair(transparent, double),
		ConfigPair(theme, int),
		ConfigPair(autoFill, bool),
		ConfigPair(focusPoint, int),
		ConfigPair(focusHide, bool),
		ConfigPair(pointMode, int),
		ConfigPair(autoCopy, bool),
		ConfigPair(lastPrompt, bool),
		ConfigPair(promptPoint, int),
		ConfigPair(keySequence, QString),
		ConfigPair(showTime, bool),
		ConfigPair(width, int),
		ConfigPair(height, int),
		ConfigPair(pluginInfo, PluginInfo*,->fileName),
		ConfigPair(autoUpdate, bool),
		ConfigPair(aiUrlKeySequence,QString),
		ConfigPair(font, QFont,.toString()),
		ConfigPair(fontSize, int),
		ConfigPair(loginDialogWidth, int),
		ConfigPair(loginDialogHeight, int),
	};
#undef ConfigPair
}

void Config::fromJson(const QJsonObject& obj, bool init)
{
	auto newObj = obj;
	transparent.setValue(newObj);
	theme.setValue(newObj);
	autoFill.setValue(newObj);
	focusPoint.setValue(newObj);
	focusHide.setValue(newObj);
	pointMode.setValue(newObj);
	autoCopy.setValue(newObj);
	lastPrompt.setValue(newObj);
	promptPoint.setValue(newObj);
	keySequence.setValue(newObj);
	showTime.setValue(newObj);
	if (newObj.contains(pluginInfo.name))
		pluginInfo.value.value<PluginInfo*>()->fileName = newObj.value(pluginInfo.name).toString();
	else
		newObj.insert(pluginInfo.name, pluginInfo.value.value<PluginInfo*>()->fileName);
	width.setValue(newObj);
	height.setValue(newObj);
	autoUpdate.setValue(newObj);
	aiUrlKeySequence.setValue(newObj);
	if (newObj.contains(font.name))
		font.value = QFont(newObj.value(font.name).toString());
	else
		newObj.insert(font.name, font.value.value<QFont>().family());
	fontSize.setValue(newObj);
	loginDialogWidth.setValue(newObj);
	loginDialogHeight.setValue(newObj);
	/*transparent.value = newObj.value(transparent.name).toDouble();
	theme.value = newObj.value(theme.name).toInt();
	autoFill.value = newObj.value(autoFill.name).toBool();
	focusPoint.value = newObj.value(focusPoint.name).toInt();
	focusHide.value = newObj.value(focusHide.name).toBool();
	pointMode.value = newObj.value(pointMode.name).toInt();
	autoCopy.value = newObj.value(autoCopy.name).toBool();
	lastPrompt.value = newObj.value(lastPrompt.name).toBool();
	promptPoint.value = newObj.value(promptPoint.name).toInt();
	keySequence.value = newObj.value(keySequence.name).toString();
	showTime.value = newObj.value(showTime.name).toBool();

	if (newObj.contains(width.name))
		width.value = newObj.value(width.name).toInt();
	else
		newObj.insert(width.name, width.value.toInt());
	if (newObj.contains(height.name))
		height.value = newObj.value(height.name).toInt();
	else
		newObj.insert(height.name, height.value.toInt());
	if (newObj.contains(autoUpdate.name))
		autoUpdate.value = newObj.value(autoUpdate.name).toBool();
	else
		newObj.insert(autoUpdate.name, autoUpdate.value.toBool());
	if (newObj.contains(aiUrlKeySequence.name))
		aiUrlKeySequence.value = newObj.value(aiUrlKeySequence.name).toString();
	else
		newObj.insert(aiUrlKeySequence.name, aiUrlKeySequence.value.toString());

	if (newObj.contains(fontSize.name))
		fontSize.value = newObj.value(fontSize.name).toInt();
	else
		newObj.insert(fontSize.name, fontSize.value.toInt());
	if (newObj.contains(loginDialogWidth.name))
		loginDialogWidth.value = newObj.value(loginDialogWidth.name).toInt();
	else
		newObj.insert(loginDialogWidth.name, loginDialogWidth.value.toInt());
	if (newObj.contains(loginDialogHeight.name))
		loginDialogHeight.value = newObj.value(loginDialogHeight.name).toInt();
	else
		newObj.insert(loginDialogHeight.name, loginDialogHeight.value.toInt());*/
	if (!init)
	{
		config()->init(QJsonDocument(newObj));
	}
}

LJsonConfig* Config::config()
{
	return _config;
}

BasePlugin* Config::plugin()
{
	QString path = QString("%1/%2/%3/%3.dll").arg(QApplication::applicationDirPath(), StrMgr::str.pluginDir, pluginInfo.value.value<PluginInfo*>()->fileName);
	QPluginLoader loader(path);
	if (QObject* plugin = loader.instance())
	{
		if (BasePluginFactory* factory = qobject_cast<BasePluginFactory*>(plugin))
		{
			BasePlugin* basePlugin = factory->create();
			basePlugin->setObjectName(pluginInfo.value.value<PluginInfo*>()->fileName);
			pluginInfo.value.value<PluginInfo*>()->name = basePlugin->getName();
			return basePlugin;
		}
	}
	return nullptr;
}

Config::Config()
{
	if (this->config()->readJson().isNull()) // 初始化防止有的人把config.json删了
		this->config()->init(QJsonDocument(this->toJson()));
	else
		this->fromJson(this->config()->readJson().object(), true);
}

Config::~Config()
{
	_config->init(QJsonDocument(this->toJson()));
	delete pluginInfo.value.value<PluginInfo*>();
	delete _config;
}

ConfigDialog::ConfigDialog(QWidget* parent)
	: QDialog(parent, Qt::WindowTitleHint | Qt::CustomizeWindowHint)
{
	ui.setupUi(this);
	ui.tabWidget->tabBar()->setStyle(new HorTabStyle());
	ui.updateLabel->setText(ui.updateLabel->text() + StrMgr::str.version);
	QDir appDir(QApplication::applicationDirPath());
	QDir dir(appDir.filePath(StrMgr::str.pluginDir));

	// 遍历里面的每一个文件夹
	QStringList pluginDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	ui.aIComboBox->addItems(pluginDirs);

	ui.transparentDoubleSpinBox->setValue(Config::instance().transparent.value.toDouble());
	ui.themeComboBox->setCurrentIndex(Config::instance().theme.value.toInt());
	ui.autoFillButton->setChecked(Config::instance().autoFill.value.toBool());
	ui.focusPointComboBox->setCurrentIndex(Config::instance().focusPoint.value.toInt());
	ui.fousButton->setChecked(Config::instance().focusHide.value.toBool());
	ui.PointComboBox->setCurrentIndex(Config::instance().pointMode.value.toInt());
	ui.autoCopyButton->setChecked(Config::instance().autoCopy.value.toBool());
	ui.lastPromptButton->setChecked(Config::instance().lastPrompt.value.toBool());
	ui.promptPointComboBox->setCurrentIndex(Config::instance().promptPoint.value.toInt());
	ui.keySequenceEdit->setKeySequence(Config::instance().keySequence.value.toString());
	ui.showTimeButton->setChecked(Config::instance().showTime.value.toBool());
	ui.aIComboBox->setCurrentText(Config::instance().pluginInfo.value.value<PluginInfo*>()->fileName);
	ui.autoUpdateButton->setChecked(Config::instance().autoUpdate.value.toBool());
	ui.AiUrlKeySequenceEdit->setKeySequence(Config::instance().aiUrlKeySequence.value.toString());
	ui.fontComboBox->setCurrentFont(Config::instance().font.value.value<QFont>());
	ui.fontSizeSpinBox->setValue(Config::instance().fontSize.value.toInt());

	connect(ui.buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &QDialog::accept);
	connect(ui.buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &QDialog::reject);
	connect(ui.pushButton, &QPushButton::clicked, [=]()
		{
			const QString path = LFunc::FString("file:///", QDir(QApplication::applicationDirPath()).filePath(StrMgr::str.promptFile));
			QDesktopServices::openUrl(QUrl(path));
		});
	connect(ui.updatePushButton, &QPushButton::clicked, [=]()
		{
			Update(true);
		});
	ui.tabWidget->setCurrentIndex(0);
}

ConfigDialog::~ConfigDialog() = default;

void ConfigDialog::accept()
{
	Config::instance().fromJson({
	{ StrMgr::str.transparent, ui.transparentDoubleSpinBox->value() },
	{ StrMgr::str.theme, ui.themeComboBox->currentIndex() },
	{ StrMgr::str.autoFill, ui.autoFillButton->isChecked() },
	{ StrMgr::str.focusPoint, ui.focusPointComboBox->currentIndex() },
	{ StrMgr::str.focusHide, ui.fousButton->isChecked() },
	{ StrMgr::str.pointMode, ui.PointComboBox->currentIndex() },
	{ StrMgr::str.autoCopy, ui.autoCopyButton->isChecked() },
	{ StrMgr::str.lastPrompt, ui.lastPromptButton->isChecked() },
	{ StrMgr::str.promptPoint, ui.promptPointComboBox->currentIndex() },
	{ StrMgr::str.keySequence, ui.keySequenceEdit->keySequence().toString() },
	{ StrMgr::str.showTime, ui.showTimeButton->isChecked() },
	{ StrMgr::str.pluginInfo, ui.aIComboBox->currentText() },
	{ StrMgr::str.autoUpdate, ui.autoUpdateButton->isChecked() },
	{ StrMgr::str.aiUrlKeySequence, ui.AiUrlKeySequenceEdit->keySequence().toString() },
	{ StrMgr::str.font, ui.fontComboBox->currentText() },
	{ StrMgr::str.fontSize, ui.fontSizeSpinBox->value() }
		});
	emit saved(false);
	QDialog::accept();
}

void HorTabStyle::drawItemText(QPainter* painter, const QRect& rect, int flags, const QPalette& pal, bool enabled,
	const QString& text, QPalette::ColorRole textRole) const
{
	if (_orientation != Qt::Horizontal)
		QProxyStyle::drawItemText(painter, rect, flags, pal, enabled, text, textRole);

	if (text.isEmpty())
		return;
	QPen savedPen;
	if (textRole != QPalette::NoRole) {
		savedPen = painter->pen();
		painter->setPen(QPen(pal.brush(textRole), savedPen.widthF()));
	}
	if (!enabled) {
		if (proxy()->styleHint(SH_DitherDisabledText)) {
			QRect br;
			this->drawHorItemText(painter, rect, flags, text);
			painter->fillRect(br, QBrush(painter->background().color(), Qt::Dense5Pattern));
			return;
		}
		else if (proxy()->styleHint(SH_EtchDisabledText)) {
			QPen pen = painter->pen();
			painter->setPen(pal.light().color());
			this->drawHorItemText(painter, rect.adjusted(1, 1, 1, 1), flags, text);
			painter->setPen(pen);
		}
	}
	this->drawHorItemText(painter, rect, flags, text);
	if (textRole != QPalette::NoRole)
		painter->setPen(savedPen);
}

void HorTabStyle::drawHorItemText(QPainter* painter, QRect rect, int flags, QString text) const
{
	auto textRect = painter->boundingRect(rect, flags, text);
	for (int i = 0; i < text.size(); ++i)
	{
		auto& c = text[text.size() - 1 - i];
		painter->save();
		painter->translate(textRect.left() + textRect.width() / text.size() * (i + 0.5), textRect.top() + textRect.height() / 2.0);
		painter->rotate(90);
		painter->drawText(-textRect.height() / 2.0, textRect.width() / text.size() / 2.0, c);
		painter->restore();
	}
}