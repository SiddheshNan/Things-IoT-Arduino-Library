#ifndef IOT_CLIENT_H
#define IOT_CLIENT_H

#include <Arduino.h>
#include <Client.h>
#include "./ArduinoJson.h"


#include "thingsiot/thingsiot.h"
#include "thingsiot/iot_msg_decode.hpp"
#include "thingsiot/iot_msg_encode.hpp"
//#include "thingsiot/iot_enums.hpp"

#ifndef SERVER_HOST
#define SERVER_HOST "10.0.0.150"
#endif

#ifndef SERVER_PORT
#define SERVER_PORT 21352
#endif

//#ifndef SERVER_PORT_SSL
//#define SERVER_PORT_SSL 21355
//#endif

//#ifndef TLS_FINGERPRINT
//    #define TLS_FINGERPRINT ""
//#endif

#ifndef RECONNECTION_TIMEOUT
#define RECONNECTION_TIMEOUT 5000   // milliseconds
#endif

#ifndef DEFAULT_READ_TIMEOUT
#define DEFAULT_READ_TIMEOUT 10000   // milliseconds
#endif

#if defined(_DEBUG_) || defined(SERIAL_DEBUG)
#define _DEBUG_
#define _DEBUG(type, text) Serial.print("["); Serial.print(F(type)); Serial.print("] "); Serial.println(F(text));
#define _DEBUG_VALUE(type, text, value) Serial.print("["); Serial.print(F(type)); Serial.print("] "); Serial.print(F(text)); Serial.println(value);
#else
#define _DEBUG(type, text) void();
#define _DEBUG_VALUE(type, text, value) void();
#endif



class ThingsiotClient
        : public thingsiot::thingsiot {
public:
    ThingsiotClient(Client &client, const char *user, const char *device, const char *device_credential) : client_(
            client),
                                                                                                           username_(
                                                                                                                   user),
                                                                                                           device_id_(
                                                                                                                   device),
                                                                                                           device_password_(
                                                                                                                   device_credential),
                                                                                                           host_(SERVER_HOST) {

    }

    ~ThingsiotClient() {

    }

    enum IOT_STATE {
        NETWORK_CONNECTING,
        NETWORK_CONNECTED,
        NETWORK_CONNECT_ERROR,
        SOCKET_CONNECTING,
        SOCKET_CONNECTED,
        SOCKET_CONNECTION_ERROR,
        SOCKET_DISCONNECTED,
        SOCKET_TIMEOUT,
        SOCKET_ERROR,
        IOT_AUTHENTICATING,
        IOT_AUTHENTICATED,
        IOT_AUTH_FAILED,
        IOT_STOP_REQUEST
    };

    virtual void state_listener(IOT_STATE state) {
#ifdef _DEBUG_
        switch(state){
            case NETWORK_CONNECTING:
                _DEBUG("NETWORK", "Starting connection...");
                break;
            case NETWORK_CONNECTED:
                _DEBUG("NETWORK", "Connected!");
                break;
            case NETWORK_CONNECT_ERROR:
                _DEBUG("NETWORK", "Cannot connect!");
                break;
            case SOCKET_CONNECTING:
                Serial.print(F("[_SOCKET] Connecting to "));
                Serial.print(host_);
                Serial.print(F(":"));
                Serial.print( SERVER_PORT);
                Serial.println(F("..."));
                Serial.print(F("[_SOCKET] Using secure TLS/SSL connection: "));
                Serial.println( F("no"));
                break;
            case SOCKET_CONNECTED:
                _DEBUG("_SOCKET", "Connected!");
                break;
            case SOCKET_CONNECTION_ERROR:
                _DEBUG("_SOCKET", "Error while connecting!");
                break;
            case SOCKET_DISCONNECTED:
                _DEBUG("_SOCKET", "Is now closed!");
                break;
            case SOCKET_ERROR:
                _DEBUG("_SOCKET", "Socket Error");
                break;
            case SOCKET_TIMEOUT:
                _DEBUG("_SOCKET", "Timeout!");
                break;
            case IOT_AUTHENTICATING:
                Serial.print(F("[ThingsIoT] Authenticating. User: "));
                Serial.print(username_);
                Serial.print(F(" Device: "));
                Serial.println(device_id_);
                break;
            case IOT_AUTHENTICATED:
                _DEBUG("ThingsIoT", "Authenticated");
                break;
            case IOT_AUTH_FAILED:
                _DEBUG("ThingsIoT", "Auth Failed! Check username, device id, or device credentials.");
                break;
            case IOT_STOP_REQUEST:
                _DEBUG("ThingsIoT", "Client was requested to stop.");
                break;
        }
#endif
    }


    void stop() {
        state_listener(IOT_STOP_REQUEST);
        client_.stop();
        state_listener(SOCKET_DISCONNECTED);
        thingsiot::thingsiot::disconnected();
    }


    void handle() {

        if (handle_connection()) {
            size_t available_bytes = client_.available();


            if (available_bytes > 0) {
                _DEBUG_VALUE("ThingsIoT", "Available bytes: ", available_bytes);

                char *buffer = (char *) malloc(available_bytes * sizeof(char));
                size_t total_read = 0;

                while (client_.available()) {
                    buffer[total_read] = (char) client_.read();
                    total_read += 1;
                }


                iot_msg_decode msg = iot_msg_decode(buffer, available_bytes);


                Serial.println(msg.get_msg_id());
                Serial.println(msg.get_msg_type());
                Serial.println(msg.get_resources());


                free(buffer);
            }
//            else {
//                _DEBUG("ThingsIoT", "No Bytes available to read");
//            }




        } else {
            delay(RECONNECTION_TIMEOUT); // get some delay for a connection retry
        }

    }


    bool handle_connection() {
        if (is_connected()) return true;

        if (!network_connected()) {
            state_listener(NETWORK_CONNECTING);

            if (!connect_network()) {
                state_listener(NETWORK_CONNECT_ERROR);
                return false;
            }

            state_listener(NETWORK_CONNECTED);

        }

        // network is connected, so connect the client
        return connect_client();
    }


    bool connect_client() {
        bool connected = false;
        client_.stop(); // cleanup previous socket

        state_listener(SOCKET_CONNECTING);

        if (connect_socket()) {
            state_listener(SOCKET_CONNECTED);
            state_listener(IOT_AUTHENTICATING);

//            connected = thingsiot::thingsiot::connect(username_, device_id_, device_password_);

            connected = connect(username_, device_id_, device_password_);

            if (!connected) {
                state_listener(IOT_AUTH_FAILED);
                client_.stop();
                state_listener(SOCKET_DISCONNECTED);
            } else {
                state_listener(IOT_AUTHENTICATED);
            }


        } else {
            state_listener(SOCKET_CONNECTION_ERROR);
        }


        return connected;
    }


    // OVERWRITTEN BY THINGISIOTWIFI.h
    virtual bool network_connected() {
        return true;
    }

    virtual bool connect_network() {
        return true;
    }
    //---------------


    virtual bool connect_socket() {
        return client_.connect(host_, SERVER_PORT);
    }


    virtual bool secure_connection() {
        return false;
    }


    bool is_connected() {
        return client_.connected();
    }

    void set_credentials(const char *username, const char *device_id, const char *device_password) {
        username_ = username;
        device_id_ = device_id;
        device_password_ = device_password;
    }

    void set_host(const char *host) {
        host_ = host;
    }

    const char *get_host() {
        return host_;
    }

    Client &get_client() {
        return client_;
    }


    bool client_write(char *_buffer, size_t size) {
#ifdef _DEBUG_
        Serial.print(F("[ThingsIoT] Writing bytes: "));
            Serial.print(size);
#endif

        size_t written = client_.write((uint8_t *) _buffer, size);
        bool success = written == size;

#ifdef _DEBUG_
        Serial.print(F(" ["));
        Serial.print(success ? F("OK") : F("FAIL"));
        Serial.println(F("]"));
        if(!success){
            _DEBUG_VALUE("ThingsIoT", "Expected:", size);
            _DEBUG_VALUE("ThingsIoT", "Wrote:", written);
        }
#endif

        return success;
    }



    bool connect(const char *username, const char *device_id, const char *credential) {

        String auth_json;

        StaticJsonDocument<200> data;
        data["username"] = username;
        data["device_id"] = device_id;
        data["credential"] = credential;

        serializeJson(data, auth_json);




//        thingsiot::client_write(auth_json.c_str(), sizeof(auth_json));

        return true;
    }



private:
    Client &client_;
    const char *username_;
    const char *device_id_;
    const char *device_password_;
    const char *host_;

    bool authenticated = false;

    uint8_t *out_buffer_;
    size_t out_size_;
    size_t out_total_size_;

};


#endif