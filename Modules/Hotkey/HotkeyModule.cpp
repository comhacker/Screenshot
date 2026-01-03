#include "HotkeyModule.h"

#include <QSettings>

#include "HotKeyManager.h"

HotkeyModule::HotkeyModule()
    : m_mgr(nullptr)
    , m_hotkeyId(1)
    , m_registered(false)
{
}

HotkeyModule::~HotkeyModule()
{
    stop();
}

bool HotkeyModule::start()
{
    if (m_mgr == nullptr) {
        m_mgr = HotKeyManager::instance();
        m_mgr->show();
        QObject::connect(m_mgr, &HotKeyManager::hotKeyPressed, [this](int id) {
            if (id == m_hotkeyId) {
                if (m_onTriggered) m_onTriggered();
            }
        });
    }
    return loadAndRegister();
}

void HotkeyModule::stop()
{
    if (m_mgr) {
        m_mgr->unregisterAllHotKeys();
    }
    m_registered = false;
}

void HotkeyModule::setTriggeredHandler(std::function<void()> handler)
{
    m_onTriggered = std::move(handler);
}

bool HotkeyModule::updateHotkey(int modifiers, int virtualKey)
{
    if (!m_mgr) return false;
    m_registered = m_mgr->registerHotKey(m_hotkeyId, modifiers, virtualKey);
    return m_registered;
}

bool HotkeyModule::loadAndRegister()
{
    QSettings settings("ScreenCapture", "Settings");
    int hotKeyModifiers = settings.value("HotKey/Modifiers", MOD_CONTROL | MOD_ALT).toInt();
    int hotKeyVirtualKey = settings.value("HotKey/VirtualKey", int('A')).toInt();
    return updateHotkey(hotKeyModifiers, hotKeyVirtualKey);
}
