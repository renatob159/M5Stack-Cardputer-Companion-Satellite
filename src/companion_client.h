/**
 * @file companion_client.h
 * @brief Bitfocus Companion WebSocket client
 */

#ifndef COMPANION_CLIENT_H
#define COMPANION_CLIENT_H

#include <Arduino.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

struct CompanionConfig {
    String host;
    uint16_t port;
    String apiKey;
};

struct ButtonState {
    uint8_t page;
    uint8_t buttonId;
    String label;
    uint16_t color;
    bool isPressed;
    String style;
};

typedef std::function<void(uint8_t, uint8_t)> ButtonEventHandler;

class CompanionClient {
public:
    CompanionClient();
    ~CompanionClient();
    
    void begin(const CompanionConfig& config);
    void update();
    void connect();
    void disconnect();
    bool isConnected();
    void reconnect();
    
    // Button actions
    void sendButtonPress(uint8_t page, uint8_t buttonId);
    void sendHeartbeat();
    
    // Event handlers
    void setButtonHandler(ButtonEventHandler handler);
    
private:
    WebSocketsClient webSocket;
    CompanionConfig config;
    ButtonEventHandler buttonHandler;
    bool connected;
    unsigned long lastReconnectAttempt;
    unsigned long lastHeartbeat;
    
    static const unsigned long RECONNECT_INTERVAL = 5000;
    static const unsigned long HEARTBEAT_INTERVAL = 30000;
    
    void parseMessage(const String& message);
    void sendSubscription();
    void handleButtonState(const JsonObject& data);
    void handleConnectionState(const JsonObject& data);
    
    // WebSocket event handlers
    void webSocketEvent(WStype_t type, uint8_t* payload, size_t length);
    static void staticWebSocketEvent(WStype_t type, uint8_t* payload, size_t length);
};

#endif // COMPANION_CLIENT_H
