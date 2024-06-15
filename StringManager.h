#pragma once

// 字符串管理类
class StrMgr
{
#define STATIC_STR(name,val) inline static const char* name = val
	/*
	 *	字符串
	 */
	struct Str
	{
		STATIC_STR(appName, "AiTools");
		STATIC_STR(quit, "退出");
		STATIC_STR(bootStart, "开机启动");
		STATIC_STR(login, "登录");
		STATIC_STR(updatePrompt, "更新提示词");
		STATIC_STR(setting, "设置");
		STATIC_STR(dateFormat, "yyyy-MM-dd HH:mm:ss");
		STATIC_STR(inPutPrompt, "请输入提示词");
		STATIC_STR(clear, "清空");
		STATIC_STR(send, "发送");
		STATIC_STR(inputData, "请输入提问的内容，按下回车发送。");
		STATIC_STR(copy, "复制");
		STATIC_STR(openLink, "打开链接");
		STATIC_STR(debug, "打开调试窗口");
		STATIC_STR(promptFile, "prompt.txt");
		STATIC_STR(textEditTip, "返回内容会在此展示，点击复制即可复制到剪切板。");
		STATIC_STR(configFile, "config.json");
		STATIC_STR(pluginDir, "Plugins");
		STATIC_STR(translationFile, "/translation/qt_zh_CN.qm");
		STATIC_STR(runArg, "show");
		STATIC_STR(transparent, "transparent");
		STATIC_STR(theme, "theme");
		STATIC_STR(autoFill, "autoFill");
		STATIC_STR(focusPoint, "focusPoint");
		STATIC_STR(focusHide, "focusHide");
		STATIC_STR(pointMode, "pointMode");
		STATIC_STR(autoCopy, "autoCopy");
		STATIC_STR(lastPrompt, "lastPrompt");
		STATIC_STR(promptPoint, "promptPoint");
		STATIC_STR(keySequence, "keySequence");
		STATIC_STR(showTime, "showTime");
		STATIC_STR(width, "width");
		STATIC_STR(height, "height");
		STATIC_STR(pluginInfo, "aiPlugin");
		STATIC_STR(defaultPluginFile, "ZhiPuAi");
		STATIC_STR(defaultPluginName, "智谱清言");
		STATIC_STR(defaultKeySequence, "Ctrl+G");
	};
	/*
	 * 资源
	 */
	struct Rc
	{
		STATIC_STR(icon, ":/AiTools/icon/AI.png");
		STATIC_STR(set, ":/AiTools/icon/set.png");
		STATIC_STR(update, ":/AiTools/icon/update.png");
		STATIC_STR(clear, ":/AiTools/icon/clear.png");
		STATIC_STR(send, ":/AiTools/icon/send.png");
		STATIC_STR(copy, ":/AiTools/icon/copy.png");
		STATIC_STR(link, ":/AiTools/icon/link.png");
		STATIC_STR(debug, ":/AiTools/icon/Debug.png");
		STATIC_STR(login, ":/AiTools/icon/login.png");
		STATIC_STR(quit, ":/AiTools/icon/exit.png");
		STATIC_STR(darkQss, ":/qdarkstyle/dark/darkstyle.qss");
		STATIC_STR(lightQss, ":/qdarkstyle/light/lightstyle.qss");
	};

	struct Color
	{
		STATIC_STR(light, "#fafafa");
		STATIC_STR(dark, "#19232D");
	};
public:
	inline static const Str str; // 字符串
	inline static const Rc rc; //资源文件
	inline static const Color color; //颜色
#undef STATIC_STR
};
