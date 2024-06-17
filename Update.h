﻿#pragma once
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
	QString updateProgram = QApplication::applicationDirPath() + "/updateProgram.exe";
	QStringList paramList;
	paramList.append(StrMgr::str.version);
	if (show)
		paramList.append("show");
	process.startDetached(updateProgram, paramList);
}
