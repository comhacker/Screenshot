#include "HotKeyManager.h"

#include <QDebug>

HotKeyManager* HotKeyManager::m_instance = nullptr;

HotKeyManager::HotKeyManager(QWidget* parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    resize(1, 1);
    move(-10000, -10000);
    winId();
    show();
}

HotKeyManager::~HotKeyManager()
{
    unregisterAllHotKeys();
}

HotKeyManager* HotKeyManager::instance()
{
    if (!m_instance) {
        m_instance = new HotKeyManager();
    }
    return m_instance;
}

bool HotKeyManager::registerHotKey(int id, int modifiers, int virtualKey)
{
    if (registeredHotKeys.contains(id)) {
        unregisterHotKey(id);
    }

    HWND hwnd = (HWND)winId();
    bool success = RegisterHotKey(hwnd, id, modifiers, virtualKey);

    if (success) {
        registeredHotKeys[id] = true;
        qDebug() << "HotKey registered:" << id << "Modifiers:" << modifiers << "VK:" << virtualKey;
    }
    else {
        DWORD error = GetLastError();
        qDebug() << "Failed to register hotkey:" << id << "Error:" << error;
    }

    return success;
}

void HotKeyManager::unregisterHotKey(int id)
{
    if (registeredHotKeys.contains(id)) {
        HWND hwnd = (HWND)winId();
        UnregisterHotKey(hwnd, id);
        registeredHotKeys.remove(id);
        qDebug() << "HotKey unregistered:" << id;
    }
}

void HotKeyManager::unregisterAllHotKeys()
{
    HWND hwnd = (HWND)winId();
    for (int id : registeredHotKeys.keys()) {
        UnregisterHotKey(hwnd, id);
    }
    registeredHotKeys.clear();
    qDebug() << "All hotkeys unregistered";
}

bool HotKeyManager::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
{
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG") {
        MSG* msg = static_cast<MSG*>(message);

        if (msg->message == WM_HOTKEY) {
            int id = msg->wParam;
            qDebug() << "HotKey pressed:" << id;
            emit hotKeyPressed(id);
            return true;
        }
    }

    return QWidget::nativeEvent(eventType, message, result);
}
