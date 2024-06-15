#pragma once
#include <QApplication>
#include <QProcess>
#include <StringManager.h>

static void Update()
{
	QProcess process;
	QString updateProgram = QApplication::applicationDirPath() + "/updateProgram.exe";
	QStringList paramList;
	paramList.append(StrMgr::str.version);
	process.startDetached(updateProgram, paramList);
}
