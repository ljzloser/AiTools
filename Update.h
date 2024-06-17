#pragma once
#include <QApplication>
#include <QProcess>
#include <StringManager.h>
/*
* @brief 调用更新程序。
* @param show 是否弹出无需更新提示。
*/
static void Update(bool show = false)
{
	QProcess process;
	QString updateProgram = QDir(QApplication::applicationDirPath()).filePath(StrMgr::str.updateProgram);
	QStringList paramList;
	paramList.append(StrMgr::str.appName);
	paramList.append(StrMgr::url.update);
	paramList.append(StrMgr::str.version);
	if (show)
		paramList.append(StrMgr::str.runArg);
	process.startDetached(updateProgram, paramList);
}
