/**
 * @file settings_manager.h
 * @brief Settings management using NVS (Non-Volatile Storage)
 */

#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>

struct WiFiConfig {
    String ssid;
    String password;
    bool dhcp;
    IPAddress staticIp;
    IPAddress gateway;
    IPAddress subnet;
};

struct CompanionConfig {
    String host;
    uint16_t port;
    String apiKey;
};

class SettingsManager {
public:
    SettingsManager();
    ~SettingsManager();
    
    void begin();
    bool hasValidSettings();
    
    // WiFi settings
    void saveWiFiConfig(const WiFiConfig& config);
    WiFiConfig getWiFiConfig();
    
    // Companion settings
    void saveCompanionConfig(const CompanionConfig& config);
    CompanionConfig getCompanionConfig();
    
    // Factory reset
    void factoryReset();
    
    // System settings
    void saveBrightness(uint8_t brightness);
    uint8_t getBrightness();
    
    void savePageCount(uint8_t pages);
    uint8_t getPageCount();
    
private:
    Preferences preferences;
    static const char* NAMESPACE;
    
    bool validateWiFiConfig(const WiFiConfig& config);
    bool validateCompanionConfig(const CompanionConfig& config);
};

#endif // SETTINGS_MANAGER_H
