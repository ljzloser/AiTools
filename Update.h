#pragma once
#include <QApplication>
#include <QProcess>
#include <StringManager.h>
/*
 * @Param show �Ƿ񵯳����������ʾ��
 */
static void Update(bool show = false)
{
	QProcess process;
	QString updateProgram = QApplication::applicationDirPath() + "/updateProgram.exe";
	QStringList paramList;
	paramList.append(StrMgr::str.version);
	if (show)
		paramList.append("show");
	process.startDetached(updateProgram, paramList);
}
