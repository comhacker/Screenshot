#include "AppController.h"

#include <QApplication>
#include <QSystemTrayIcon>
#include <QWidget>

#include "../Tray/TrayModule.h"
#include "../Hotkey/HotkeyModule.h"

#include "../../App/Lang.h"
#include "../Tray/TrayIcon.h"
#include "../../Win/WinFull.h"
#include "../../Win/WinSettings.h"

namespace {
    TrayModule* g_tray = nullptr;
    HotkeyModule* g_hotkey = nullptr;
    bool g_running = false;
}

static void openCapture()
{
    new WinFull();
}

static void openSettings()
{
    WinSettings* settingsWin = new WinSettings();
    settingsWin->setAttribute(Qt::WA_DeleteOnClose, true);
    QObject::connect(settingsWin, &WinSettings::hotKeyChanged, [](int modifiers, int virtualKey) {
        if (g_hotkey) {
            g_hotkey->updateHotkey(modifiers, virtualKey);
        }
    });
    settingsWin->show();
}

void AppController::startTrayMode()
{
    if (g_running) return;

    qApp->setQuitOnLastWindowClosed(false);

    g_tray = new TrayModule();
    g_tray->start();

    g_hotkey = new HotkeyModule();
    g_hotkey->setTriggeredHandler([]() {
        openCapture();
    });
    const bool ok = g_hotkey->start();

    if (g_tray->trayIcon()) {
        QObject::connect(g_tray->trayIcon(), &TrayIcon::captureRequested, []() {
            openCapture();
        });
        QObject::connect(g_tray->trayIcon(), &TrayIcon::settingsRequested, []() {
            openSettings();
        });

        if (!ok) {
            g_tray->trayIcon()->showMessage("ScreenCapture", Lang::get("hotkeyFailed"), QSystemTrayIcon::Warning, 3000);
        }
    }

    g_running = true;
}

void AppController::stop()
{
    if (!g_running) return;

    if (g_hotkey) {
        g_hotkey->stop();
        delete g_hotkey;
        g_hotkey = nullptr;
    }

    if (g_tray) {
        g_tray->stop();
        delete g_tray;
        g_tray = nullptr;
    }

    g_running = false;
}

bool AppController::isRunning()
{
    return g_running;
}
