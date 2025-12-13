#include "TrayIcon.h"
#include "Lang.h"
#include <QApplication>
#include <QMessageBox>
#include <QIcon>

TrayIcon::TrayIcon(QObject* parent)
    : QSystemTrayIcon(parent)
    , hotKeyId(1)
    , hotKeyModifiers(MOD_CONTROL | MOD_ALT)
    , hotKeyVirtualKey(int('A'))
{
    settings = new QSettings("ScreenCapture", "Settings", this);
    
    // 设置托盘图标
    QIcon trayIco = QIcon(":/Res/logo.ico");
    if (trayIco.isNull()) {
        trayIco = QIcon("Res/logo.ico");
    }
    setIcon(trayIco);
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::warning(nullptr, "ScreenCapture", "System tray is not available.");
    }
    setToolTip("ScreenCapture - " + Lang::get("readyToCapture"));
    
    // 创建菜单
    createMenu();
    
    // 连接信号
    connect(this, &QSystemTrayIcon::activated, this, &TrayIcon::onActivated);
    
    // 加载热键配置
    loadHotKeyFromSettings();
    
    // 显示托盘图标
    show();
    
    // 显示启动提示
    showMessage(
        "ScreenCapture",
        Lang::get("trayStarted") + "\n" + Lang::get("hotkey") + ": " + getHotKeyString(),
        QSystemTrayIcon::Information,
        2000
    );
}

TrayIcon::~TrayIcon()
{
    unregisterGlobalHotKey();
}

void TrayIcon::createMenu()
{
    menu = new QMenu();
    
    // 截图动作
    captureAction = new QAction(Lang::get("capture"), this);
    captureAction->setIcon(QIcon(":/Res/logo.ico"));
    connect(captureAction, &QAction::triggered, this, &TrayIcon::onCaptureAction);
    
    // 设置动作
    settingsAction = new QAction(Lang::get("settings"), this);
    connect(settingsAction, &QAction::triggered, this, &TrayIcon::onSettingsAction);
    
    // 关于动作
    aboutAction = new QAction(Lang::get("about"), this);
    connect(aboutAction, &QAction::triggered, this, &TrayIcon::onAboutAction);
    
    // 退出动作
    exitAction = new QAction(Lang::get("exit"), this);
    connect(exitAction, &QAction::triggered, this, &TrayIcon::onExitAction);
    
    // 添加到菜单
    menu->addAction(captureAction);
    menu->addSeparator();
    menu->addAction(settingsAction);
    menu->addAction(aboutAction);
    menu->addSeparator();
    menu->addAction(exitAction);
    
    setContextMenu(menu);
}

void TrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        // 单击或双击托盘图标时触发截图
        emit captureRequested();
    }
}

void TrayIcon::onCaptureAction()
{
    emit captureRequested();
}

void TrayIcon::onSettingsAction()
{
    emit settingsRequested();
}

void TrayIcon::onAboutAction()
{
    QMessageBox::about(nullptr, 
        "ScreenCapture",
        QString("<h2>ScreenCapture v2.3.3</h2>") +
        "<p>" + Lang::get("aboutText") + "</p>" +
        "<p><a href='https://github.com/xland/ScreenCapture'>GitHub</a></p>" +
        "<p>" + Lang::get("hotkey") + ": " + getHotKeyString() + "</p>"
    );
}

void TrayIcon::onExitAction()
{
    qApp->quit();
}

bool TrayIcon::registerGlobalHotKey(int modifiers, int key)
{
    // 先注销旧的热键
    unregisterGlobalHotKey();
    
    // 注册新的热键
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) {
        hwnd = GetDesktopWindow();
    }
    
    bool success = RegisterHotKey(hwnd, hotKeyId, modifiers, key);
    
    if (success) {
        hotKeyModifiers = modifiers;
        hotKeyVirtualKey = key;
        saveHotKeyToSettings();
        
        showMessage(
            "ScreenCapture",
            Lang::get("hotkeyRegistered") + ": " + getHotKeyString(),
            QSystemTrayIcon::Information,
            2000
        );
    } else {
        showMessage(
            "ScreenCapture",
            Lang::get("hotkeyFailed"),
            QSystemTrayIcon::Warning,
            3000
        );
    }
    
    return success;
}

void TrayIcon::unregisterGlobalHotKey()
{
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) {
        hwnd = GetDesktopWindow();
    }
    UnregisterHotKey(hwnd, hotKeyId);
}

QString TrayIcon::getHotKeyString() const
{
    QString result;
    
    if (hotKeyModifiers & MOD_CONTROL) {
        result += "Ctrl+";
    }
    if (hotKeyModifiers & MOD_ALT) {
        result += "Alt+";
    }
    if (hotKeyModifiers & MOD_SHIFT) {
        result += "Shift+";
    }
    if (hotKeyModifiers & MOD_WIN) {
        result += "Win+";
    }
    
    // 将虚拟键码转换为字符
    if (hotKeyVirtualKey >= 'A' && hotKeyVirtualKey <= 'Z') {
        result += QChar(hotKeyVirtualKey);
    } else if (hotKeyVirtualKey >= VK_F1 && hotKeyVirtualKey <= VK_F12) {
        result += QString("F%1").arg(hotKeyVirtualKey - VK_F1 + 1);
    } else {
        result += QString("Key%1").arg(hotKeyVirtualKey);
    }
    
    return result;
}

void TrayIcon::loadHotKeyFromSettings()
{
    hotKeyModifiers = settings->value("HotKey/Modifiers", MOD_CONTROL | MOD_ALT).toInt();
    hotKeyVirtualKey = settings->value("HotKey/VirtualKey", int('A')).toInt();
    
    registerGlobalHotKey(hotKeyModifiers, hotKeyVirtualKey);
}

void TrayIcon::saveHotKeyToSettings()
{
    settings->setValue("HotKey/Modifiers", hotKeyModifiers);
    settings->setValue("HotKey/VirtualKey", hotKeyVirtualKey);
    settings->sync();
}

void TrayIcon::loadSettings()
{
    loadHotKeyFromSettings();
}

