/**
 * @file wifi_manager.cpp
 * @brief WiFi Manager implementation
 */

#include "wifi_manager.h"

WiFiManager::WiFiManager() {
    connected = false;
    lastReconnectAttempt = 0;
    connectionStartTime = 0;
}

WiFiManager::~WiFiManager() {
    disconnect();
}

void WiFiManager::begin() {
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    Serial.println("WiFi manager initialized");
}

void WiFiManager::update() {
    if (!connected && millis() - lastReconnectAttempt > RECONNECT_INTERVAL) {
        reconnect();
        lastReconnectAttempt = millis();
    }
    
    if (connected && WiFi.status() != WL_CONNECTED) {
        connected = false;
        Serial.println("WiFi connection lost");
    }
}

bool WiFiManager::connect() {
    return connectToWiFi();
}

void WiFiManager::disconnect() {
    WiFi.disconnect(true);
    connected = false;
}

bool WiFiManager::isConnected() {
    return connected && (WiFi.status() == WL_CONNECTED);
}

void WiFiManager::reconnect() {
    if (!connected) {
        Serial.println("Attempting WiFi reconnection...");
        connectToWiFi();
    }
}

void WiFiManager::setConfig(const WiFiConfig& config) {
    this->config = config;
}

WiFiConfig WiFiManager::getConfig() {
    return config;
}

bool WiFiManager::connectToWiFi() {
    if (config.ssid.length() == 0) {
        Serial.println("No WiFi credentials available");
        return false;
    }
    
    Serial.printf("Connecting to WiFi: %s\n", config.ssid.c_str());
    
    if (!config.dhcp) {
        setupStaticIP();
    }
    
    WiFi.begin(config.ssid.c_str(), config.password.c_str());
    connectionStartTime = millis();
    
    while (millis() - connectionStartTime < CONNECTION_TIMEOUT) {
        if (WiFi.status() == WL_CONNECTED) {
            connected = true;
            printNetworkInfo();
            return true;
        }
        delay(500);
    }
    
    Serial.println("WiFi connection timeout");
    connected = false;
    return false;
}

void WiFiManager::setupStaticIP() {
    if (!config.dhcp) {
        WiFi.config(config.staticIp, config.gateway, config.subnet);
        Serial.println("Static IP configured");
    }
}

void WiFiManager::printNetworkInfo() {
    Serial.println("WiFi connected!");
    Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
    Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
    Serial.printf("Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
}

String WiFiManager::getLocalIP() {
    return WiFi.localIP().toString();
}

int8_t WiFiManager::getRSSI() {
    return WiFi.RSSI();
}

String WiFiManager::getSSID() {
    return WiFi.SSID();
}
