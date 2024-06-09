#include "AiTools.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
int main(int argc, char* argv[])
{
	_putenv_s("QT_FONT_DPI", "96");
	QApplication a(argc, argv);

	QTranslator translator;
	if (translator.load(QApplication::applicationDirPath() + "\\translations\\qt_zh_CN.qm")) {
		a.installTranslator(&translator);
	}
	LTitleBar* titleBar = new LTitleBar();
	titleBar->setTitleIcon(QIcon());
	titleBar->setTitleText("");
	AiTools* tool = new AiTools();
	LWidget widget(titleBar, tool);
	widget.setWindowIcon(QIcon(":/AiTools/icon/AI.png"));
	QObject::connect(&widget, &LWidget::systemSettingsChanged, tool, &AiTools::loadConfig);
	tool->loadConfig();

	widget.setWindowFlags(widget.windowFlags() | Qt::SubWindow | Qt::WindowStaysOnTopHint);
	widget.resize(300, 200);
	titleBar->getCloseButton()->hide();
	titleBar->getMaxButton()->hide();
	titleBar->getMinButton()->hide();
	return a.exec();
}