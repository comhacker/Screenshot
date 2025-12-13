#pragma once

#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QSettings>
#include <Windows.h>

class TrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    TrayIcon(QObject* parent = nullptr);
    ~TrayIcon();
    
    // 注册全局热键
    bool registerGlobalHotKey(int modifiers, int key);
    void unregisterGlobalHotKey();
    
    // 获取当前热键配置
    QString getHotKeyString() const;
    
    // 从配置加载热键
    void loadHotKeyFromSettings();
    void saveHotKeyToSettings();

signals:
    void captureRequested();  // 请求截图
    void settingsRequested(); // 请求打开设置

private slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason);
    void onCaptureAction();
    void onSettingsAction();
    void onAboutAction();
    void onExitAction();

private:
    void createMenu();
    void loadSettings();
    
    QMenu* menu;
    QAction* captureAction;
    QAction* settingsAction;
    QAction* aboutAction;
    QAction* exitAction;
    
    // 热键相关
    int hotKeyId;
    int hotKeyModifiers;  // MOD_CONTROL, MOD_ALT, MOD_SHIFT, MOD_WIN
    int hotKeyVirtualKey; // VK_A, VK_F1, etc.
    
    QSettings* settings;
};



