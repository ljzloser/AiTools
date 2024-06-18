#include "AiTools.h"
#include <QTranslator>
#include <LWidget>
#include <Update.h>
#include <QFontDatabase>

static QStringList GetFontFiles()
{
	QDir dir = QDir(QApplication::applicationDirPath()).filePath("Font");
	QStringList fontFiles = dir.entryList(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	for (int i = 0; i < fontFiles.size(); i++)
	{
		if (fontFiles[i].endsWith(".ttf") || fontFiles[i].endsWith(".otf")
			|| fontFiles[i].endsWith(".TTF") || fontFiles[i].endsWith(".OTF"))
		{
			fontFiles[i] = dir.filePath(fontFiles[i]);
		}
		else
		{
			fontFiles.removeAt(i);
			i--;
		}
	}
	return fontFiles;
}
int main(int argc, char* argv[])
{
	_putenv_s("QT_FONT_DPI", "96");

	// 获取启动参数
	bool isShow = false;
	if (argc > 1)
	{
		isShow = strcmp(argv[1], StrMgr::str.runArg.toStdString().c_str()) == 0;
	}

	LSingleApplication app(argc, argv);
	if (app.getInstanceRunning())
		return 0;
	for (auto&& font : GetFontFiles())
	{
		QFontDatabase::addApplicationFont(font);
	}
	QTranslator translator;
	if (translator.load(QApplication::applicationDirPath() + StrMgr::str.translationFile))
		app.installTranslator(&translator);
	TitleBar* titleBar = new TitleBar();
	AiTools* tool = new AiTools();
	Widget widget(titleBar, tool);
	app.setMainWindow(&widget);
	app.setQuitOnLastWindowClosed(false);

	if (isShow)
	{
		widget.show();
	}
	if (Config::instance().autoUpdate.value.toBool())
		Update();
	return app.exec();
}