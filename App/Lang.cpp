#include <QApplication>

#include "Lang.h"

Lang* lang;

Lang::Lang(QObject *parent) : QObject(parent)
{
	
}

Lang::~Lang()
{

}

QString Lang::get(const QString& name)
{
	return lang->dic[name];
}

void Lang::init(const QString& langType)
{
	lang = new Lang(qApp);
	if (langType == "zhcn") {
		lang->initZhCn();
	}
	else if (langType == "en") {
		lang->initEn();
	}
}

void Lang::initZhCn()
{
	dic.insert("saveFile", "保存文件");

	dic.insert("rect", "矩形");
	dic.insert("ellipse", "圆形");
	dic.insert("arrow", "箭头");
	dic.insert("number", "标号");
	dic.insert("line", "线条");
	dic.insert("text", "文本");
	dic.insert("mosaic", "马赛克");
	dic.insert("eraser", "橡皮擦");
	dic.insert("undo", "撤销");
	dic.insert("redo", "重做");
	dic.insert("pin", "钉住");
	dic.insert("clipboard", "保存到剪切板");
	dic.insert("save", "保存");
	dic.insert("close", "关闭");

	dic.insert("rectFill", "填充矩形");
	dic.insert("strokeCtrl", "线条粗细：");
	dic.insert("ellipseFill", "填充椭圆");
	dic.insert("arrowFill", "填充箭头");
	dic.insert("numberFill", "填充标号");
	dic.insert("lineTransparent", "半透明线条");
	dic.insert("bold", "粗体");
	dic.insert("italic", "斜体");
	dic.insert("mosaicFill", "矩形马赛克");
	dic.insert("eraserFill", "矩形橡皮擦");

	dic.insert("red", "红");
	dic.insert("yellow", "黄");
	dic.insert("green", "绿");
	dic.insert("cyan", "青");
	dic.insert("blue", "蓝");
	dic.insert("purple", "紫");
	dic.insert("pink", "粉");
	dic.insert("black", "黑");

	dic.insert("start", "开始");
	dic.insert("reachBottom", "已触底，自动滚动停止");
	dic.insert("tooLong", "长图过长，已自动停止");
	
	// 托盘和设置相关
	dic.insert("readyToCapture", "就绪，点击或按快捷键截图");
	dic.insert("trayStarted", "程序已在后台运行");
	dic.insert("hotkey", "快捷键");
	dic.insert("capture", "截图");
	dic.insert("settings", "设置");
	dic.insert("about", "关于");
	dic.insert("exit", "退出");
	dic.insert("hotkeyRegistered", "快捷键已注册");
	dic.insert("hotkeyFailed", "快捷键注册失败，可能与其他程序冲突");
	dic.insert("aboutText", "一个功能强大的截图工具");
	dic.insert("success", "成功");
	dic.insert("settingsSaved", "设置已保存！快捷键将在下次启动时生效。");
	dic.insert("save", "保存");
	dic.insert("cancel", "取消");
	dic.insert("resetDefault", "恢复默认");
	dic.insert("hotkeySettings", "快捷键设置");
	dic.insert("captureHotkey", "截图快捷键");
	dic.insert("hotkeyHint", "提示：推荐使用 Ctrl+Alt+字母 组合，避免与系统快捷键冲突");
	dic.insert("generalSettings", "常规设置");
	dic.insert("autoStartWithWindows", "开机自动启动");
	dic.insert("showTrayNotifications", "显示托盘通知");
	dic.insert("runInBackground", "后台运行");
}

void Lang::initEn()
{
	dic.insert("saveFile", "Save File");
	dic.insert("rect", "Rect");
	dic.insert("ellipse", "Ellipse");
	dic.insert("arrow", "Arrow");
	dic.insert("number", "Number");
	dic.insert("line", "Pen");
	dic.insert("text", "Text");
	dic.insert("mosaic", "Mosaic");
	dic.insert("eraser", "Eraser");
	dic.insert("undo", "Undo");
	dic.insert("redo", "Redo");
	dic.insert("pin", "Pin");
	dic.insert("clipboard", "Save to Clipboard");
	dic.insert("save", "Save to Disk");
	dic.insert("close", "Close");

	dic.insert("rectFill", "Filled Rect");
	dic.insert("strokeCtrl", "Stroke Width: ");
	dic.insert("ellipseFill", "Filled Ellipse");
	dic.insert("arrowFill", "Filled Arrow");
	dic.insert("numberFill", "Filled Number");
	dic.insert("lineTransparent", "Translucent Line");
	dic.insert("bold", "Bold");
	dic.insert("italic", "Italics");
	dic.insert("mosaicFill", "Rectangle Mosaic");
	dic.insert("eraserFill", "Rectangle Eraser");


	dic.insert("red", "Red");
	dic.insert("yellow", "Yellow");
	dic.insert("green", "Green");
	dic.insert("cyan", "Cyan");
	dic.insert("blue", "Blue");
	dic.insert("purple", "Purple");
	dic.insert("pink", "Pink");
	dic.insert("black", "Black");

	dic.insert("start", "Start");
	dic.insert("reachBottom", "Reached the bottom, auto scrolling stopped");
	dic.insert("tooLong", "Image is too long，auto scrolling stopped");
	
	// Tray and settings
	dic.insert("readyToCapture", "Ready, click or press hotkey to capture");
	dic.insert("trayStarted", "Running in background");
	dic.insert("hotkey", "Hotkey");
	dic.insert("capture", "Capture");
	dic.insert("settings", "Settings");
	dic.insert("about", "About");
	dic.insert("exit", "Exit");
	dic.insert("hotkeyRegistered", "Hotkey registered");
	dic.insert("hotkeyFailed", "Failed to register hotkey, may conflict with other programs");
	dic.insert("aboutText", "A powerful screenshot tool");
	dic.insert("success", "Success");
	dic.insert("settingsSaved", "Settings saved! Hotkey will take effect on next startup.");
	dic.insert("save", "Save");
	dic.insert("cancel", "Cancel");
	dic.insert("resetDefault", "Reset to Default");
	dic.insert("hotkeySettings", "Hotkey Settings");
	dic.insert("captureHotkey", "Capture Hotkey");
	dic.insert("hotkeyHint", "Tip: Use Ctrl+Alt+Letter combinations to avoid conflicts with system hotkeys");
	dic.insert("generalSettings", "General Settings");
	dic.insert("autoStartWithWindows", "Start with Windows");
	dic.insert("showTrayNotifications", "Show tray notifications");
	dic.insert("runInBackground", "Run in background");
}
