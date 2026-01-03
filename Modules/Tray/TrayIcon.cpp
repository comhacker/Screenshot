#include "TrayIcon.h"

#include "../../App/Lang.h"

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

    QIcon trayIco = QIcon(":/Res/logo.ico");
    if (trayIco.isNull()) {
        trayIco = QIcon("Res/logo.ico");
    }
    setIcon(trayIco);
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::warning(nullptr, "ScreenCapture", "System tray is not available.");
    }
    setToolTip("ScreenCapture - " + Lang::get("readyToCapture"));

    createMenu();

    connect(this, &QSystemTrayIcon::activated, this, &TrayIcon::onActivated);

    loadHotKeyFromSettings();

    show();

    showMessage(
        "ScreenCapture",
        Lang::get("trayStarted") + "\n" + Lang::get("hotkey") + ": " + getHotKeyString(),
        QSystemTrayIcon::Information,
        2000
    );
}

TrayIcon::~TrayIcon()
{
}

void TrayIcon::createMenu()
{
    menu = new QMenu();

    captureAction = new QAction(Lang::get("capture"), this);
    captureAction->setIcon(QIcon(":/Res/logo.ico"));
    connect(captureAction, &QAction::triggered, this, &TrayIcon::onCaptureAction);

    settingsAction = new QAction(Lang::get("settings"), this);
    connect(settingsAction, &QAction::triggered, this, &TrayIcon::onSettingsAction);

    aboutAction = new QAction(Lang::get("about"), this);
    connect(aboutAction, &QAction::triggered, this, &TrayIcon::onAboutAction);

    exitAction = new QAction(Lang::get("exit"), this);
    connect(exitAction, &QAction::triggered, this, &TrayIcon::onExitAction);

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
    hotKeyModifiers = modifiers;
    hotKeyVirtualKey = key;
    saveHotKeyToSettings();
    return true;
}

void TrayIcon::unregisterGlobalHotKey()
{
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

    if (hotKeyVirtualKey >= 'A' && hotKeyVirtualKey <= 'Z') {
        result += QChar(hotKeyVirtualKey);
    }
    else if (hotKeyVirtualKey >= VK_F1 && hotKeyVirtualKey <= VK_F12) {
        result += QString("F%1").arg(hotKeyVirtualKey - VK_F1 + 1);
    }
    else {
        result += QString("Key%1").arg(hotKeyVirtualKey);
    }

    return result;
}

void TrayIcon::loadHotKeyFromSettings()
{
    hotKeyModifiers = settings->value("HotKey/Modifiers", MOD_CONTROL | MOD_ALT).toInt();
    hotKeyVirtualKey = settings->value("HotKey/VirtualKey", int('A')).toInt();
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
