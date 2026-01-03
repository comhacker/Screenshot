#pragma once

#include <functional>

class HotKeyManager;

class HotkeyModule
{
public:
    HotkeyModule();
    ~HotkeyModule();

    bool start();
    void stop();

    bool updateHotkey(int modifiers, int virtualKey);
    void setTriggeredHandler(std::function<void()> handler);

private:
    bool loadAndRegister();

private:
    HotKeyManager* m_mgr;
    std::function<void()> m_onTriggered;
    int m_hotkeyId;
    bool m_registered;
};
