/**
 * @file main.cpp
 * @brief M5Stack Cardputer Companion Satellite Controller
 * @version 1.0.0
 * 
 * Main entry point for the Companion Satellite firmware
 */

#include <Arduino.h>
#include <M5Cardputer.h>
#include "wifi_manager.h"
#include "companion_client.h"
#include "ui_manager.h"
#include "keyboard_manager.h"
#include "settings_manager.h"

// System state
enum SystemState {
    STATE_INIT,
    STATE_SETUP_MODE,
    STATE_CONNECTING_WIFI,
    STATE_CONNECTING_COMPANION,
    STATE_RUNNING,
    STATE_ERROR
};

static SystemState currentState = STATE_INIT;
static unsigned long lastHeartbeat = 0;
static const unsigned long HEARTBEAT_INTERVAL = 5000;

// Task handles
TaskHandle_t uiTaskHandle = NULL;
TaskHandle_t networkTaskHandle = NULL;

// Global managers
WiFiManager wifiManager;
CompanionClient companionClient;
UIManager uiManager;
KeyboardManager keyboardManager;
SettingsManager settingsManager;

// Function prototypes
void setupSystem();
void handleSystemState();
void updateSystemStatus();
void checkHeartbeat();
void uiTask(void *parameter);
void networkTask(void *parameter);

/**
 * @brief Arduino setup function
 */
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n=== Companion Satellite v1.0 ===");
    Serial.println("Starting system initialization...");
    
    setupSystem();
    
    // Create RTOS tasks
    xTaskCreatePinnedToCore(
        uiTask,
        "UI Task",
        8192,
        NULL,
        2,
        &uiTaskHandle,
        0
    );
    
    xTaskCreatePinnedToCore(
        networkTask,
        "Network Task",
        4096,
        NULL,
        1,
        &networkTaskHandle,
        1
    );
    
    Serial.println("System ready!");
}

/**
 * @brief System initialization
 */
void setupSystem() {
    // Initialize M5Stack Cardputer
    M5Cardputer.begin();
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextColor(WHITE);
    M5Cardputer.Display.setTextSize(2);
    
    // Show splash screen
    uiManager.showSplash();
    delay(2000);
    
    // Initialize settings
    settingsManager.begin();
    
    // Initialize keyboard
    keyboardManager.begin();
    keyboardManager.setKeyHandler([](KeyCode key, KeyEvent event) {
        if (event == KEY_PRESS) {
            uiManager.handleKeyPress(key);
        }
    });
    
    // Check if settings exist
    if (!settingsManager.hasValidSettings()) {
        Serial.println("No valid settings found. Entering setup mode...");
        currentState = STATE_SETUP_MODE;
        uiManager.showSetupScreen();
    } else {
        currentState = STATE_CONNECTING_WIFI;
        uiManager.showConnectingScreen();
        
        // Load and connect WiFi
        WiFiConfig wifiConfig = settingsManager.getWiFiConfig();
        wifiManager.begin();
        wifiManager.setConfig(wifiConfig);
        
        if (wifiManager.connect()) {
            // Load and connect to Companion
            CompanionConfig companionConfig = settingsManager.getCompanionConfig();
            companionClient.begin(companionConfig);
            companionClient.setButtonHandler([](int page, int buttonId) {
                // Handle button press from Companion
                uiManager.updateButtonState(page, buttonId);
            });
            
            if (companionClient.connect()) {
                currentState = STATE_RUNNING;
                uiManager.showMainScreen();
            } else {
                currentState = STATE_ERROR;
                uiManager.showError("Companion connection failed");
            }
        } else {
            currentState = STATE_ERROR;
            uiManager.showError("WiFi connection failed");
        }
    }
}

/**
 * @brief Main system state handler
 */
void handleSystemState() {
    switch (currentState) {
        case STATE_SETUP_MODE:
            // Handle setup mode
            if (keyboardManager.isKeyPressed(KEY_ENTER)) {
                // Save settings and restart
                WiFiConfig wifiConfig = uiManager.getWiFiSetupConfig();
                CompanionConfig companionConfig = uiManager.getCompanionSetupConfig();
                
                settingsManager.saveWiFiConfig(wifiConfig);
                settingsManager.saveCompanionConfig(companionConfig);
                
                ESP.restart();
            }
            break;
            
        case STATE_RUNNING:
            // Update UI with Companion status
            updateSystemStatus();
            checkHeartbeat();
            
            // Handle button presses from keyboard
            if (keyboardManager.isKeyPressed(KEY_ENTER)) {
                int currentPage = uiManager.getCurrentPage();
                int selectedButton = uiManager.getSelectedButton();
                companionClient.sendButtonPress(currentPage, selectedButton);
                uiManager.animateButtonPress(selectedButton);
            }
            
            // Page navigation
            if (keyboardManager.isKeyPressed(KEY_LEFT)) {
                uiManager.previousPage();
            }
            if (keyboardManager.isKeyPressed(KEY_RIGHT)) {
                uiManager.nextPage();
            }
            
            // Settings menu
            if (keyboardManager.isKeyPressed(KEY_MENU)) {
                currentState = STATE_SETUP_MODE;
                uiManager.showSetupScreen();
            }
            break;
            
        case STATE_ERROR:
            // Attempt recovery after delay
            delay(5000);
            ESP.restart();
            break;
            
        default:
            break;
    }
}

/**
 * @brief Update system status display
 */
void updateSystemStatus() {
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 1000) {
        uiManager.updateConnectionStatus(
            wifiManager.isConnected(),
            companionClient.isConnected()
        );
        lastUpdate = millis();
    }
}

/**
 * @brief Check and send heartbeat to Companion
 */
void checkHeartbeat() {
    if (millis() - lastHeartbeat > HEARTBEAT_INTERVAL) {
        companionClient.sendHeartbeat();
        lastHeartbeat = millis();
    }
}

/**
 * @brief UI RTOS Task
 */
void uiTask(void *parameter) {
    while (true) {
        uiManager.update();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Network RTOS Task
 */
void networkTask(void *parameter) {
    while (true) {
        handleSystemState();
        
        if (currentState == STATE_RUNNING) {
            wifiManager.update();
            companionClient.update();
            
            // Handle reconnection
            if (!wifiManager.isConnected()) {
                Serial.println("WiFi disconnected. Attempting reconnect...");
                wifiManager.reconnect();
            }
            
            if (!companionClient.isConnected()) {
                Serial.println("Companion disconnected. Attempting reconnect...");
                companionClient.reconnect();
            }
        }
        
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Arduino loop function (minimal - RTOS handles everything)
 */
void loop() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
