/**
 * @file keyboard_manager.cpp
 * @brief Keyboard Manager implementation
 */

#include "keyboard_manager.h"

KeyboardManager::KeyboardManager() {
    lastKey = KEY_NONE;
    lastKeyTime = 0;
    memset(keyStates, 0, sizeof(keyStates));
}

KeyboardManager::~KeyboardManager() {}

void KeyboardManager::begin() {
    M5Cardputer.Keyboard.begin();
    Serial.println("Keyboard initialized");
}

void KeyboardManager::update() {
    M5Cardputer.update();
    
    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState key = M5Cardputer.Keyboard.keysState();
            KeyCode code = mapM5KeyToCode(key);
            
            if (code != KEY_NONE && !keyStates[code]) {
                processKey(code, KEY_PRESS);
                keyStates[code] = true;
            }
        } else {
            // Key release
            for (int i = 0; i < 128; i++) {
                if (keyStates[i]) {
                    processKey((KeyCode)i, KEY_RELEASE);
                    keyStates[i] = false;
                }
            }
        }
    }
}

void KeyboardManager::setKeyHandler(KeyEventHandler handler) {
    keyHandler = handler;
}

bool KeyboardManager::isKeyPressed(KeyCode key) {
    return keyStates[key];
}

KeyCode KeyboardManager::getLastKey() {
    KeyCode key = lastKey;
    lastKey = KEY_NONE;
    return key;
}

KeyCode KeyboardManager::mapM5KeyToCode(Keyboard_Class::KeysState key) {
    // Map M5Stack Cardputer keyboard to our KeyCode enum
    switch (key.which()) {
        case KEY_UP_ARROW: return KEY_UP;
        case KEY_DOWN_ARROW: return KEY_DOWN;
        case KEY_LEFT_ARROW: return KEY_LEFT;
        case KEY_RIGHT_ARROW: return KEY_RIGHT;
        case KEY_ENTER: return KEY_ENTER;
        case KEY_ESC: return KEY_ESC;
        case KEY_F1: return KEY_MENU;
        
        // Number keys
        case KEY_1: return KEY_1;
        case KEY_2: return KEY_2;
        case KEY_3: return KEY_3;
        case KEY_4: return KEY_4;
        case KEY_5: return KEY_5;
        case KEY_6: return KEY_6;
        case KEY_7: return KEY_7;
        case KEY_8: return KEY_8;
        case KEY_9: return KEY_9;
        case KEY_0: return KEY_0;
        
        // Letter keys
        case KEY_A: return KEY_A;
        case KEY_B: return KEY_B;
        case KEY_C: return KEY_C;
        case KEY_D: return KEY_D;
        case KEY_E: return KEY_E;
        case KEY_F: return KEY_F;
        case KEY_G: return KEY_G;
        case KEY_H: return KEY_H;
        case KEY_I: return KEY_I;
        case KEY_J: return KEY_J;
        case KEY_K: return KEY_K;
        case KEY_L: return KEY_L;
        case KEY_M: return KEY_M;
        case KEY_N: return KEY_N;
        case KEY_O: return KEY_O;
        case KEY_P: return KEY_P;
        case KEY_Q: return KEY_Q;
        case KEY_R: return KEY_R;
        case KEY_S: return KEY_S;
        case KEY_T: return KEY_T;
        case KEY_U: return KEY_U;
        case KEY_V: return KEY_V;
        case KEY_W: return KEY_W;
        case KEY_X: return KEY_X;
        case KEY_Y: return KEY_Y;
        case KEY_Z: return KEY_Z;
        
        default: return KEY_NONE;
    }
}

void KeyboardManager::processKey(KeyCode code, KeyEvent event) {
    if (keyHandler) {
        keyHandler(code, event);
    }
    
    if (event == KEY_PRESS) {
        lastKey = code;
        lastKeyTime = millis();
        
        // Debug output
        Serial.print("Key pressed: ");
        Serial.println(code);
    }
}

void KeyboardManager::debounceKey(KeyCode code) {
    // Simple debounce delay
    delay(50);
}
