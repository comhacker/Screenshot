#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QKeySequenceEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QSpinBox>

class WinSettings : public QDialog
{
    Q_OBJECT

public:
    WinSettings(QWidget* parent = nullptr);
    ~WinSettings();

signals:
    void hotKeyChanged(int modifiers, int virtualKey);
    void settingsSaved();

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onResetClicked();
    void onKeySequenceChanged(const QKeySequence& keySequence);

private:
    void createUI();
    void loadSettings();
    void saveSettings();
    int qtKeyToWinVK(int qtKey);
    int qtModifiersToWinMod(Qt::KeyboardModifiers qtMod);

    // UI 组件
    QVBoxLayout* mainLayout;
    
    // 热键设置组
    QGroupBox* hotKeyGroup;
    QLabel* hotKeyLabel;
    QKeySequenceEdit* hotKeyEdit;
    QLabel* hotKeyHintLabel;
    
    // 其他设置组
    QGroupBox* generalGroup;
    QCheckBox* autoStartCheck;
    QCheckBox* showTrayMessageCheck;
    
    // 按钮
    QHBoxLayout* buttonLayout;
    QPushButton* saveButton;
    QPushButton* cancelButton;
    QPushButton* resetButton;
    
    // 当前热键
    int currentModifiers;
    int currentVirtualKey;
};



