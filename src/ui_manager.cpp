/**
 * @file ui_manager.cpp
 * @brief UI Manager implementation
 */

#include "ui_manager.h"
#include "assets/fonts.h"

UIManager::UIManager() {
    currentPage = 0;
    selectedButton = 0;
    totalPages = 3;
    wifiStatus = false;
    companionStatus = false;
    isSetupMode = false;
    setupStep = 0;
    backgroundColor = TFT_BLACK;
    canvas = nullptr;
}

UIManager::~UIManager() {
    if (canvas) delete canvas;
}

void UIManager::begin() {
    canvas = new M5Canvas(&M5Cardputer.Display);
    canvas->setColorDepth(16);
    canvas->createSprite(240, 135);
    canvas->setTextSize(1);
    canvas->setTextFont(2);
    
    initButtons();
}

void UIManager::initButtons() {
    int buttonWidth = 42;
    int buttonHeight = 38;
    int startX = 6;
    int startY = 25;
    int spacing = 4;
    
    for (int page = 0; page < MAX_PAGES; page++) {
        for (int row = 0; row < BUTTONS_PER_COL; row++) {
            for (int col = 0; col < BUTTONS_PER_ROW; col++) {
                int idx = row * BUTTONS_PER_ROW + col;
                if (idx >= MAX_BUTTONS) break;
                
                buttons[page][idx].x = startX + col * (buttonWidth + spacing);
                buttons[page][idx].y = startY + row * (buttonHeight + spacing);
                buttons[page][idx].width = buttonWidth;
                buttons[page][idx].height = buttonHeight;
                buttons[page][idx].label = "Btn " + String(idx + 1);
                buttons[page][idx].color = TFT_DARKGREY;
                buttons[page][idx].textColor = TFT_WHITE;
                buttons[page][idx].isPressed = false;
                buttons[page][idx].isHighlighted = false;
                buttons[page][idx].page = page;
                buttons[page][idx].id = idx;
            }
        }
    }
}

void UIManager::showSplash() {
    clearScreen();
    canvas->fillSprite(TFT_BLACK);
    canvas->setTextSize(2);
    canvas->setTextColor(TFT_CYAN);
    canvas->setCursor(30, 50);
    canvas->println("Companion");
    canvas->setCursor(45, 75);
    canvas->println("Satellite");
    canvas->setTextSize(1);
    canvas->setTextColor(TFT_WHITE);
    canvas->setCursor(20, 110);
    canvas->println("Loading v1.0...");
    canvas->pushSprite(0, 0);
    delay(200);
}

void UIManager::showSetupScreen() {
    isSetupMode = true;
    setupStep = 0;
    clearScreen();
    
    canvas->fillSprite(TFT_BLACK);
    canvas->setTextColor(TFT_WHITE);
    canvas->setTextSize(1);
    
    // Draw setup form
    canvas->drawRect(10, 10, 220, 115, TFT_CYAN);
    canvas->setCursor(15, 20);
    canvas->println("SETUP MODE");
    
    canvas->setCursor(15, 40);
    canvas->println("WiFi SSID:");
    canvas->setCursor(15, 55);
    canvas->println(tempSSID);
    
    canvas->setCursor(15, 70);
    canvas->println("WiFi Password:");
    canvas->setCursor(15, 85);
    canvas->println("********");
    
    canvas->setCursor(15, 100);
    canvas->println("IP: " + tempHost);
    canvas->setCursor(15, 115);
    canvas->println("Port: " + String(tempPort));
    
    canvas->pushSprite(0, 0);
}

void UIManager::showMainScreen() {
    isSetupMode = false;
    clearScreen();
    updateButtonGrid();
    drawStatusBar();
    drawPageIndicator();
}

void UIManager::updateButtonGrid() {
    for (int i = 0; i < MAX_BUTTONS; i++) {
        drawButton(buttons[currentPage][i]);
    }
    
    // Highlight selected button
    buttons[currentPage][selectedButton].isHighlighted = true;
    drawButton(buttons[currentPage][selectedButton]);
}

void UIManager::drawButton(const Button& button, bool forceRedraw) {
    uint16_t color = button.color;
    if (button.isPressed) {
        color = TFT_NAVY;
    } else if (button.isHighlighted) {
        color = TFT_BLUE;
    }
    
    // Draw button background
    canvas->fillRoundRect(button.x, button.y, button.width, button.height, 4, color);
    canvas->drawRoundRect(button.x, button.y, button.width, button.height, 4, TFT_WHITE);
    
    // Draw label
    canvas->setTextColor(button.textColor);
    canvas->setTextSize(1);
    int textWidth = button.label.length() * 6;
    int textX = button.x + (button.width - textWidth) / 2;
    int textY = button.y + (button.height - 8) / 2;
    canvas->setCursor(textX, textY);
    canvas->println(button.label);
    
    canvas->pushSprite(0, 0);
    
    // Reset press animation
    if (button.isPressed) {
        delay(50);
        button.isPressed = false;
        drawButton(button);
    }
}

void UIManager::drawStatusBar() {
    // Draw status bar background
    canvas->fillRect(0, 0, 240, 18, TFT_DARKGREY);
    
    // WiFi icon and status
    canvas->setTextSize(1);
    canvas->setTextColor(wifiStatus ? TFT_GREEN : TFT_RED);
    canvas->setCursor(5, 4);
    canvas->print("WiFi");
    
    // Companion status
    canvas->setCursor(45, 4);
    canvas->setTextColor(companionStatus ? TFT_GREEN : TFT_RED);
    canvas->print("Cmp");
    
    // Battery indicator (placeholder)
    canvas->setCursor(200, 4);
    canvas->setTextColor(TFT_WHITE);
    canvas->print("100%");
    
    canvas->pushSprite(0, 0);
}

void UIManager::drawPageIndicator() {
    int indicatorY = 122;
    int dotSize = 4;
    int spacing = 8;
    int startX = (240 - (totalPages * spacing)) / 2;
    
    for (int i = 0; i < totalPages; i++) {
        uint16_t color = (i == currentPage) ? TFT_CYAN : TFT_DARKGREY;
        canvas->fillCircle(startX + i * spacing, indicatorY, dotSize, color);
    }
    
    canvas->pushSprite(0, 0);
}

void UIManager::animateButtonPress(uint8_t buttonId) {
    if (buttonId < MAX_BUTTONS) {
        buttons[currentPage][buttonId].isPressed = true;
        drawButton(buttons[currentPage][buttonId]);
    }
}

void UIManager::updateButtonState(uint8_t page, uint8_t buttonId) {
    if (page < MAX_PAGES && buttonId < MAX_BUTTONS) {
        // Update button color based on Companion state
        buttons[page][buttonId].color = getButtonColor(buttonId);
        drawButton(buttons[page][buttonId]);
    }
}

void UIManager::updateConnectionStatus(bool wifiConnected, bool companionConnected) {
    wifiStatus = wifiConnected;
    companionStatus = companionConnected;
    drawStatusBar();
}

void UIManager::handleKeyPress(KeyCode key) {
    if (isSetupMode) {
        // Handle setup mode navigation
        switch (key) {
            case KEY_UP:
                setupStep = (setupStep > 0) ? setupStep - 1 : 0;
                break;
            case KEY_DOWN:
                setupStep = (setupStep < 3) ? setupStep + 1 : 3;
                break;
            case KEY_ENTER:
                // Save current step
                showSetupScreen();
                break;
        }
    } else {
        // Handle main screen navigation
        switch (key) {
            case KEY_UP:
                if (selectedButton >= BUTTONS_PER_ROW) {
                    buttons[currentPage][selectedButton].isHighlighted = false;
                    drawButton(buttons[currentPage][selectedButton]);
                    selectedButton -= BUTTONS_PER_ROW;
                    buttons[currentPage][selectedButton].isHighlighted = true;
                    drawButton(buttons[currentPage][selectedButton]);
                }
                break;
                
            case KEY_DOWN:
                if (selectedButton + BUTTONS_PER_ROW < MAX_BUTTONS) {
                    buttons[currentPage][selectedButton].isHighlighted = false;
                    drawButton(buttons[currentPage][selectedButton]);
                    selectedButton += BUTTONS_PER_ROW;
                    buttons[currentPage][selectedButton].isHighlighted = true;
                    drawButton(buttons[currentPage][selectedButton]);
                }
                break;
                
            case KEY_LEFT:
                if (selectedButton % BUTTONS_PER_ROW > 0) {
                    buttons[currentPage][selectedButton].isHighlighted = false;
                    drawButton(buttons[currentPage][selectedButton]);
                    selectedButton--;
                    buttons[currentPage][selectedButton].isHighlighted = true;
                    drawButton(buttons[currentPage][selectedButton]);
                }
                break;
                
            case KEY_RIGHT:
                if (selectedButton % BUTTONS_PER_ROW < BUTTONS_PER_ROW - 1) {
                    buttons[currentPage][selectedButton].isHighlighted = false;
                    drawButton(buttons[currentPage][selectedButton]);
                    selectedButton++;
                    buttons[currentPage][selectedButton].isHighlighted = true;
                    drawButton(buttons[currentPage][selectedButton]);
                }
                break;
        }
    }
}

uint8_t UIManager::getSelectedButton() {
    return selectedButton;
}

uint8_t UIManager::getCurrentPage() {
    return currentPage;
}

void UIManager::nextPage() {
    if (currentPage + 1 < totalPages) {
        currentPage++;
        updateButtonGrid();
        drawPageIndicator();
    }
}

void UIManager::previousPage() {
    if (currentPage > 0) {
        currentPage--;
        updateButtonGrid();
        drawPageIndicator();
    }
}

void UIManager::clearScreen() {
    canvas->fillSprite(backgroundColor);
    canvas->pushSprite(0, 0);
}

void UIManager::showConnectingScreen() {
    clearScreen();
    canvas->fillSprite(TFT_BLACK);
    canvas->setTextColor(TFT_YELLOW);
    canvas->setCursor(30, 60);
    canvas->println("Connecting...");
    canvas->pushSprite(0, 0);
}

void UIManager::showError(const String& message) {
    clearScreen();
    canvas->fillSprite(TFT_BLACK);
    canvas->setTextColor(TFT_RED);
    canvas->setCursor(10, 50);
    canvas->println("ERROR:");
    canvas->setCursor(10, 70);
    canvas->println(message);
    canvas->setCursor(10, 90);
    canvas->println("Restarting...");
    canvas->pushSprite(0, 0);
}

uint16_t UIManager::getButtonColor(uint8_t state) {
    // Map Companion button states to colors
    switch (state) {
        case 0: return TFT_DARKGREY;  // Off
        case 1: return TFT_GREEN;     // On
        case 2: return TFT_RED;       // Error
        default: return TFT_DARKGREY;
    }
}

WiFiConfig UIManager::getWiFiSetupConfig() {
    WiFiConfig config;
    config.ssid = tempSSID;
    config.password = tempPassword;
    config.dhcp = true;
    return config;
}

CompanionConfig UIManager::getCompanionSetupConfig() {
    CompanionConfig config;
    config.host = tempHost;
    config.port = tempPort;
    return config;
}
