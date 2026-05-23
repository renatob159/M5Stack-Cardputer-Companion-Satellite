/**
 * @file keyboard_manager.h
 * @brief Keyboard input management for M5Stack Cardputer
 */

#ifndef KEYBOARD_MANAGER_H
#define KEYBOARD_MANAGER_H

#include <Arduino.h>
#include <M5Cardputer.h>

enum KeyCode {
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ENTER,
    KEY_ESC,
    KEY_MENU,
    KEY_1, KEY_2, KEY_3, KEY_4, KEY_5,
    KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E,
    KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
    KEY_K, KEY_L, KEY_M, KEY_N, KEY_O,
    KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
    KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z
};

enum KeyEvent {
    KEY_PRESS,
    KEY_RELEASE,
    KEY_HOLD
};

typedef std::function<void(KeyCode, KeyEvent)> KeyEventHandler;

class KeyboardManager {
public:
    KeyboardManager();
    ~KeyboardManager();
    
    void begin();
    void update();
    void setKeyHandler(KeyEventHandler handler);
    
    bool isKeyPressed(KeyCode key);
    KeyCode getLastKey();
    
private:
    KeyEventHandler keyHandler;
    KeyCode lastKey;
    unsigned long lastKeyTime;
    bool keyStates[128];
    
    KeyCode mapM5KeyToCode(Keyboard_Class::KeysState key);
    void processKey(KeyCode code, KeyEvent event);
    void debounceKey(KeyCode code);
};

#endif // KEYBOARD_MANAGER_H
