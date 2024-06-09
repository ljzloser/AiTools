#include "AiTools.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include <LWidget>

int main(int argc, char* argv[])
{
	_putenv_s("QT_FONT_DPI", "96");
	QApplication a(argc, argv);
	QTranslator translator;
	if (translator.load(QApplication::applicationDirPath() + "\\translations\\qt_zh_CN.qm"))
		a.installTranslator(&translator);
	TitleBar* titleBar = new TitleBar();
	AiTools* tool = new AiTools();
	Widget widget(titleBar, tool);
	widget.resize(300, 200);
	return a.exec();
}