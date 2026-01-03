#pragma once

#include <QWidget>
#include <QMap>
#include <Windows.h>

class HotKeyManager : public QWidget
{
    Q_OBJECT

public:
    static HotKeyManager* instance();

    bool registerHotKey(int id, int modifiers, int virtualKey);

    void unregisterHotKey(int id);

    void unregisterAllHotKeys();

signals:
    void hotKeyPressed(int id);

protected:
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;

private:
    HotKeyManager(QWidget* parent = nullptr);
    ~HotKeyManager();

    static HotKeyManager* m_instance;
    QMap<int, bool> registeredHotKeys;
};
