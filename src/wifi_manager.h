/**
 * @file wifi_manager.h
 * @brief WiFi connection management with auto-reconnect
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "settings_manager.h"

class WiFiManager {
public:
    WiFiManager();
    ~WiFiManager();
    
    void begin();
    void update();
    
    bool connect();
    void disconnect();
    bool isConnected();
    void reconnect();
    
    void setConfig(const WiFiConfig& config);
    WiFiConfig getConfig();
    
    String getLocalIP();
    int8_t getRSSI();
    String getSSID();
    
private:
    WiFiConfig config;
    bool connected;
    unsigned long lastReconnectAttempt;
    unsigned long connectionStartTime;
    
    static const unsigned long RECONNECT_INTERVAL = 10000;
    static const unsigned long CONNECTION_TIMEOUT = 15000;
    
    bool connectToWiFi();
    void setupStaticIP();
    void printNetworkInfo();
};

#endif // WIFI_MANAGER_H
