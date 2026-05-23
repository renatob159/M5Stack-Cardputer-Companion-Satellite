/**
 * @file ui_manager.h
 * @brief UI management with LVGL-like interface using M5GFX
 */

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <M5Cardputer.h>
#include "keyboard_manager.h"

struct Button {
    int x, y;
    int width, height;
    String label;
    uint16_t color;
    uint16_t textColor;
    bool isPressed;
    bool isHighlighted;
    uint8_t page;
    uint8_t id;
};

class UIManager {
public:
    UIManager();
    ~UIManager();
    
    void begin();
    void update();
    
    // Screen management
    void showSplash();
    void showSetupScreen();
    void showConnectingScreen();
    void showMainScreen();
    void showError(const String& message);
    
    // Button management
    void updateButtonState(uint8_t page, uint8_t buttonId);
    void animateButtonPress(uint8_t buttonId);
    void setSelectedButton(uint8_t buttonId);
    uint8_t getSelectedButton();
    
    // Navigation
    void nextPage();
    void previousPage();
    uint8_t getCurrentPage();
    
    // Status updates
    void updateConnectionStatus(bool wifiConnected, bool companionConnected);
    
    // Event handling
    void handleKeyPress(KeyCode key);
    
    // Setup mode data
    WiFiConfig getWiFiSetupConfig();
    CompanionConfig getCompanionSetupConfig();
    
private:
    static const uint8_t BUTTONS_PER_ROW = 5;
    static const uint8_t BUTTONS_PER_COL = 3;
    static const uint8_t MAX_BUTTONS = BUTTONS_PER_ROW * BUTTONS_PER_COL;
    static const uint8_t MAX_PAGES = 10;
    
    Button buttons[MAX_PAGES][MAX_BUTTONS];
    uint8_t currentPage;
    uint8_t selectedButton;
    uint8_t totalPages;
    
    bool wifiStatus;
    bool companionStatus;
    bool isSetupMode;
    
    // UI Elements
    M5Canvas* canvas;
    uint16_t backgroundColor;
    
    // Setup mode variables
    String tempSSID;
    String tempPassword;
    String tempHost;
    uint16_t tempPort;
    uint8_t setupStep;
    
    // Private methods
    void drawButton(const Button& button, bool forceRedraw = false);
    void drawStatusBar();
    void drawPageIndicator();
    void clearScreen();
    void initButtons();
    void updateButtonGrid();
    uint16_t getButtonColor(uint8_t state);
    
    // Drawing helpers
    void drawRoundedRect(int x, int y, int w, int h, uint16_t color, int radius);
    void drawIcon(uint8_t iconId, int x, int y);
};

#endif // UI_MANAGER_H
