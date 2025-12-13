#pragma once

#include <QWidget>
#include <QMap>
#include <Windows.h>

class HotKeyManager : public QWidget
{
    Q_OBJECT

public:
    static HotKeyManager* instance();
    
    // 注册全局热键
    bool registerHotKey(int id, int modifiers, int virtualKey);
    
    // 注销热键
    void unregisterHotKey(int id);
    
    // 注销所有热键
    void unregisterAllHotKeys();

signals:
    void hotKeyPressed(int id);

protected:
    // Windows 消息处理
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;

private:
    HotKeyManager(QWidget* parent = nullptr);
    ~HotKeyManager();
    
    static HotKeyManager* m_instance;
    QMap<int, bool> registeredHotKeys;
};



