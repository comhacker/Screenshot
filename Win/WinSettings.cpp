#include "WinSettings.h"
#include "App/Lang.h"
#include <QSettings>
#include <QMessageBox>
#include <Windows.h>

WinSettings::WinSettings(QWidget* parent)
    : QDialog(parent)
    , currentModifiers(MOD_CONTROL | MOD_ALT)
    , currentVirtualKey(int('A'))
{
    setWindowTitle(Lang::get("settings"));
    setWindowIcon(QIcon(":/Res/logo.ico"));
    setMinimumWidth(450);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    
    createUI();
    loadSettings();
}

WinSettings::~WinSettings()
{
}

void WinSettings::createUI()
{
    mainLayout = new QVBoxLayout(this);
    
    // ========== 热键设置组 ==========
    hotKeyGroup = new QGroupBox(Lang::get("hotkeySettings"), this);
    QVBoxLayout* hotKeyLayout = new QVBoxLayout(hotKeyGroup);
    
    hotKeyLabel = new QLabel(Lang::get("captureHotkey") + ":", this);
    hotKeyLayout->addWidget(hotKeyLabel);
    
    hotKeyEdit = new QKeySequenceEdit(this);
    hotKeyEdit->setMaximumSequenceLength(1);
    connect(hotKeyEdit, &QKeySequenceEdit::keySequenceChanged, 
            this, &WinSettings::onKeySequenceChanged);
    hotKeyLayout->addWidget(hotKeyEdit);
    
    hotKeyHintLabel = new QLabel(Lang::get("hotkeyHint"), this);
    hotKeyHintLabel->setWordWrap(true);
    hotKeyHintLabel->setStyleSheet("color: gray; font-size: 11px;");
    hotKeyLayout->addWidget(hotKeyHintLabel);
    
    mainLayout->addWidget(hotKeyGroup);
    
    // ========== 常规设置组 ==========
    generalGroup = new QGroupBox(Lang::get("generalSettings"), this);
    QVBoxLayout* generalLayout = new QVBoxLayout(generalGroup);
    
    autoStartCheck = new QCheckBox(Lang::get("autoStartWithWindows"), this);
    generalLayout->addWidget(autoStartCheck);
    
    showTrayMessageCheck = new QCheckBox(Lang::get("showTrayNotifications"), this);
    showTrayMessageCheck->setChecked(true);
    generalLayout->addWidget(showTrayMessageCheck);
    
    mainLayout->addWidget(generalGroup);
    
    // ========== 按钮组 ==========
    buttonLayout = new QHBoxLayout();
    
    resetButton = new QPushButton(Lang::get("resetDefault"), this);
    connect(resetButton, &QPushButton::clicked, this, &WinSettings::onResetClicked);
    buttonLayout->addWidget(resetButton);
    
    buttonLayout->addStretch();
    
    saveButton = new QPushButton(Lang::get("save"), this);
    saveButton->setDefault(true);
    connect(saveButton, &QPushButton::clicked, this, &WinSettings::onSaveClicked);
    buttonLayout->addWidget(saveButton);
    
    cancelButton = new QPushButton(Lang::get("cancel"), this);
    connect(cancelButton, &QPushButton::clicked, this, &WinSettings::onCancelClicked);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    setLayout(mainLayout);
}

void WinSettings::loadSettings()
{
    QSettings settings("ScreenCapture", "Settings");
    
    // 加载热键
    currentModifiers = settings.value("HotKey/Modifiers", MOD_CONTROL | MOD_ALT).toInt();
    currentVirtualKey = settings.value("HotKey/VirtualKey", int('A')).toInt();
    
    // 转换为 Qt 键序列显示
    Qt::KeyboardModifiers qtMod = Qt::NoModifier;
    if (currentModifiers & MOD_CONTROL) qtMod |= Qt::ControlModifier;
    if (currentModifiers & MOD_ALT) qtMod |= Qt::AltModifier;
    if (currentModifiers & MOD_SHIFT) qtMod |= Qt::ShiftModifier;
    if (currentModifiers & MOD_WIN) qtMod |= Qt::MetaModifier;
    
    int qtKey = currentVirtualKey; // 简化处理，大部分ASCII码相同
    if (currentVirtualKey >= VK_F1 && currentVirtualKey <= VK_F12) {
        qtKey = Qt::Key_F1 + (currentVirtualKey - VK_F1);
    }
    
    QKeySequence keySeq(QKeyCombination(qtMod, static_cast<Qt::Key>(qtKey)));
    hotKeyEdit->setKeySequence(keySeq);
    
    // 加载其他设置
    autoStartCheck->setChecked(settings.value("General/AutoStart", false).toBool());
    showTrayMessageCheck->setChecked(settings.value("General/ShowTrayMessage", true).toBool());
}

void WinSettings::saveSettings()
{
    QSettings settings("ScreenCapture", "Settings");
    
    settings.setValue("HotKey/Modifiers", currentModifiers);
    settings.setValue("HotKey/VirtualKey", currentVirtualKey);
    settings.setValue("General/AutoStart", autoStartCheck->isChecked());
    settings.setValue("General/ShowTrayMessage", showTrayMessageCheck->isChecked());
    settings.sync();
    
    emit settingsSaved();
}

void WinSettings::onSaveClicked()
{
    saveSettings();
    
    // 发送热键改变信号
    emit hotKeyChanged(currentModifiers, currentVirtualKey);
    
    QMessageBox::information(this, 
        Lang::get("success"), 
        Lang::get("settingsSaved"));
    
    accept();
}

void WinSettings::onCancelClicked()
{
    reject();
}

void WinSettings::onResetClicked()
{
    // 重置为默认值
    currentModifiers = MOD_CONTROL | MOD_ALT;
    currentVirtualKey = int('A');
    
    QKeySequence keySeq(QKeyCombination(Qt::ControlModifier | Qt::AltModifier, Qt::Key_A));
    hotKeyEdit->setKeySequence(keySeq);
    
    autoStartCheck->setChecked(false);
    showTrayMessageCheck->setChecked(true);
}

void WinSettings::onKeySequenceChanged(const QKeySequence& keySequence)
{
    if (keySequence.isEmpty()) {
        return;
    }
    
    // 获取第一个按键组合（Qt6: QKeyCombination）
    QKeyCombination combo = keySequence[0];
    
    // 提取修饰键
    Qt::KeyboardModifiers qtMod = combo.keyboardModifiers();
    currentModifiers = qtModifiersToWinMod(qtMod);
    
    // 提取主键
    int qtKey = static_cast<int>(combo.key());
    currentVirtualKey = qtKeyToWinVK(qtKey);
}

int WinSettings::qtKeyToWinVK(int qtKey)
{
    // F功能键
    if (qtKey >= Qt::Key_F1 && qtKey <= Qt::Key_F12) {
        return VK_F1 + (qtKey - Qt::Key_F1);
    }
    
    // 字母和数字大部分相同
    if (qtKey >= Qt::Key_A && qtKey <= Qt::Key_Z) {
        return qtKey;
    }
    
    if (qtKey >= Qt::Key_0 && qtKey <= Qt::Key_9) {
        return qtKey;
    }
    
    // 其他特殊键映射
    switch (qtKey) {
        case Qt::Key_Space: return VK_SPACE;
        case Qt::Key_Return:
        case Qt::Key_Enter: return VK_RETURN;
        case Qt::Key_Escape: return VK_ESCAPE;
        case Qt::Key_Tab: return VK_TAB;
        case Qt::Key_Backspace: return VK_BACK;
        case Qt::Key_Insert: return VK_INSERT;
        case Qt::Key_Delete: return VK_DELETE;
        case Qt::Key_Home: return VK_HOME;
        case Qt::Key_End: return VK_END;
        case Qt::Key_PageUp: return VK_PRIOR;
        case Qt::Key_PageDown: return VK_NEXT;
        case Qt::Key_Left: return VK_LEFT;
        case Qt::Key_Up: return VK_UP;
        case Qt::Key_Right: return VK_RIGHT;
        case Qt::Key_Down: return VK_DOWN;
        default: return qtKey;
    }
}

int WinSettings::qtModifiersToWinMod(Qt::KeyboardModifiers qtMod)
{
    int winMod = 0;
    
    if (qtMod & Qt::ControlModifier) winMod |= MOD_CONTROL;
    if (qtMod & Qt::AltModifier) winMod |= MOD_ALT;
    if (qtMod & Qt::ShiftModifier) winMod |= MOD_SHIFT;
    if (qtMod & Qt::MetaModifier) winMod |= MOD_WIN;
    
    return winMod;
}

