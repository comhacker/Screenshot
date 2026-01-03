#pragma once

class TrayIcon;

class TrayModule
{
public:
    TrayModule();
    ~TrayModule();

    bool start();
    void stop();

    TrayIcon* trayIcon() const;

private:
    TrayIcon* m_trayIcon;
};
