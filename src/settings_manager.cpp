/**
 * @file settings_manager.cpp
 * @brief Settings management implementation
 */

#include "settings_manager.h"

const char* SettingsManager::NAMESPACE = "companion";

SettingsManager::SettingsManager() {}

SettingsManager::~SettingsManager() {
    preferences.end();
}

void SettingsManager::begin() {
    preferences.begin(NAMESPACE, false);
}

bool SettingsManager::hasValidSettings() {
    return preferences.isKey("wifi_ssid") && 
           preferences.isKey("companion_host");
}

void SettingsManager::saveWiFiConfig(const WiFiConfig& config) {
    preferences.putString("wifi_ssid", config.ssid);
    preferences.putString("wifi_pass", config.password);
    preferences.putBool("wifi_dhcp", config.dhcp);
    
    if (!config.dhcp) {
        preferences.putUInt("wifi_ip", config.staticIp);
        preferences.putUInt("wifi_gw", config.gateway);
        preferences.putUInt("wifi_mask", config.subnet);
    }
    
    Serial.println("WiFi configuration saved");
}

WiFiConfig SettingsManager::getWiFiConfig() {
    WiFiConfig config;
    config.ssid = preferences.getString("wifi_ssid", "");
    config.password = preferences.getString("wifi_pass", "");
    config.dhcp = preferences.getBool("wifi_dhcp", true);
    
    if (!config.dhcp) {
        config.staticIp = preferences.getUInt("wifi_ip");
        config.gateway = preferences.getUInt("wifi_gw");
        config.subnet = preferences.getUInt("wifi_mask");
    }
    
    return config;
}

void SettingsManager::saveCompanionConfig(const CompanionConfig& config) {
    preferences.putString("companion_host", config.host);
    preferences.putUInt("companion_port", config.port);
    preferences.putString("companion_key", config.apiKey);
    
    Serial.println("Companion configuration saved");
}

CompanionConfig SettingsManager::getCompanionConfig() {
    CompanionConfig config;
    config.host = preferences.getString("companion_host", "192.168.1.100");
    config.port = preferences.getUInt("companion_port", 8000);
    config.apiKey = preferences.getString("companion_key", "");
    
    return config;
}

void SettingsManager::factoryReset() {
    preferences.clear();
    Serial.println("Factory reset performed");
}

void SettingsManager::saveBrightness(uint8_t brightness) {
    brightness = constrain(brightness, 0, 255);
    preferences.putUChar("brightness", brightness);
}

uint8_t SettingsManager::getBrightness() {
    return preferences.getUChar("brightness", 128);
}

void SettingsManager::savePageCount(uint8_t pages) {
    pages = constrain(pages, 1, 10);
    preferences.putUChar("pages", pages);
}

uint8_t SettingsManager::getPageCount() {
    return preferences.getUChar("pages", 3);
}

bool SettingsManager::validateWiFiConfig(const WiFiConfig& config) {
    if (config.ssid.length() == 0) return false;
    if (!config.dhcp && config.staticIp == INADDR_NONE) return false;
    return true;
}

bool SettingsManager::validateCompanionConfig(const CompanionConfig& config) {
    if (config.host.length() == 0) return false;
    if (config.port == 0 || config.port > 65535) return false;
    return true;
}
