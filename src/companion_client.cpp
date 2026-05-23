/**
 * @file companion_client.cpp
 * @brief Companion Client implementation
 */

#include "companion_client.h"

static CompanionClient* instance = nullptr;

CompanionClient::CompanionClient() {
    connected = false;
    lastReconnectAttempt = 0;
    lastHeartbeat = 0;
    instance = this;
}

CompanionClient::~CompanionClient() {
    disconnect();
    instance = nullptr;
}

void CompanionClient::begin(const CompanionConfig& config) {
    this->config = config;
    
    webSocket.begin(config.host, config.port);
    webSocket.onEvent([](WStype_t type, uint8_t* payload, size_t length) {
        if (instance) instance->webSocketEvent(type, payload, length);
    });
    webSocket.setReconnectInterval(5000);
    
    Serial.println("Companion client initialized");
}

void CompanionClient::update() {
    webSocket.loop();
    
    // Send heartbeat
    if (connected && millis() - lastHeartbeat > HEARTBEAT_INTERVAL) {
        sendHeartbeat();
        lastHeartbeat = millis();
    }
    
    // Reconnect logic
    if (!connected && millis() - lastReconnectAttempt > RECONNECT_INTERVAL) {
        reconnect();
        lastReconnectAttempt = millis();
    }
}

void CompanionClient::connect() {
    webSocket.begin(config.host, config.port);
    Serial.printf("Connecting to Companion at %s:%d\n", config.host.c_str(), config.port);
}

void CompanionClient::disconnect() {
    webSocket.disconnect();
    connected = false;
}

bool CompanionClient::isConnected() {
    return connected;
}

void CompanionClient::reconnect() {
    if (!connected) {
        connect();
    }
}

void CompanionClient::sendButtonPress(uint8_t page, uint8_t buttonId) {
    if (!connected) return;
    
    JsonDocument doc;
    doc["type"] = "press";
    doc["page"] = page;
    doc["button"] = buttonId;
    
    String output;
    serializeJson(doc, output);
    webSocket.sendTXT(output);
    
    Serial.printf("Button press sent: page=%d, button=%d\n", page, buttonId);
}

void CompanionClient::sendHeartbeat() {
    if (!connected) return;
    
    JsonDocument doc;
    doc["type"] = "heartbeat";
    doc["timestamp"] = millis();
    
    String output;
    serializeJson(doc, output);
    webSocket.sendTXT(output);
}

void CompanionClient::sendSubscription() {
    if (!connected) return;
    
    JsonDocument doc;
    doc["type"] = "subscribe";
    doc["scope"] = "buttons";
    
    String output;
    serializeJson(doc, output);
    webSocket.sendTXT(output);
    
    Serial.println("Subscribed to button events");
}

void CompanionClient::parseMessage(const String& message) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    
    if (error) {
        Serial.print("JSON parse error: ");
        Serial.println(error.c_str());
        return;
    }
    
    const char* type = doc["type"];
    
    if (strcmp(type, "button_state") == 0) {
        handleButtonState(doc.as<JsonObject>());
    } else if (strcmp(type, "connection_state") == 0) {
        handleConnectionState(doc.as<JsonObject>());
    }
}

void CompanionClient::handleButtonState(const JsonObject& data) {
    uint8_t page = data["page"];
    uint8_t buttonId = data["button"];
    
    if (buttonHandler) {
        buttonHandler(page, buttonId);
    }
}

void CompanionClient::handleConnectionState(const JsonObject& data) {
    bool isConnected = data["connected"];
    Serial.printf("Companion connection state changed: %d\n", isConnected);
}

void CompanionClient::webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("Disconnected from Companion");
            connected = false;
            break;
            
        case WStype_CONNECTED:
            Serial.println("Connected to Companion");
            connected = true;
            sendSubscription();
            break;
            
        case WStype_TEXT:
            if (length > 0) {
                String message = String((char*)payload);
                parseMessage(message);
            }
            break;
            
        case WStype_ERROR:
            Serial.println("WebSocket error");
            connected = false;
            break;
            
        default:
            break;
    }
}

void CompanionClient::setButtonHandler(ButtonEventHandler handler) {
    buttonHandler = handler;
}
