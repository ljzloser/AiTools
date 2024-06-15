#include "AiTools.h"
#include <QTranslator>
#include <LWidget>
#include <Update.h>

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
	Update();
	return app.exec();
}