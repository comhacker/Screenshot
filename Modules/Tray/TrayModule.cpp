#include "TrayModule.h"

#include "TrayIcon.h"

TrayModule::TrayModule()
    : m_trayIcon(nullptr)
{
}

TrayModule::~TrayModule()
{
    stop();
}

bool TrayModule::start()
{
    if (m_trayIcon != nullptr) return true;
    m_trayIcon = new TrayIcon();
    return true;
}

void TrayModule::stop()
{
    if (m_trayIcon) {
        delete m_trayIcon;
        m_trayIcon = nullptr;
    }
}

TrayIcon* TrayModule::trayIcon() const
{
    return m_trayIcon;
}
