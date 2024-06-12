#include "AiTools.h"
#include <QTranslator>
#include <LWidget>
#include <QMessageBox>

int main(int argc, char* argv[])
{
	_putenv_s("QT_FONT_DPI", "96");
	LSingleApplication app(argc, argv);
	if (app.getInstanceRunning())
		return 0;
	QTranslator translator;
	if (translator.load(QApplication::applicationDirPath() + "\\translations\\qt_zh_CN.qm"))
		app.installTranslator(&translator);
	TitleBar* titleBar = new TitleBar();
	AiTools* tool = new AiTools();
	Widget widget(titleBar, tool);
	app.setMainWindow(&widget);
	app.setQuitOnLastWindowClosed(false);

	return app.exec();
}