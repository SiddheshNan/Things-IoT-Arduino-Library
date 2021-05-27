#include "../protobuf/pb_common.h"
#include "../protobuf/message.pb.h"
#include "../protobuf/pb.h"
#include "../protobuf/pb_encode.h"
#include "../protobuf/pb_decode.h"

#include "./iot_enums.hpp"

uint8_t check = 0;
String msg_id = "";
String resource = "";
String value = "";


bool decode_msg_(pb_istream_t *stream, const pb_field_t *field, void **arg) {
    uint8_t buffer[1024] = {0};

    if (stream->bytes_left > sizeof(buffer) - 1) {
        Serial.println("Failed1");
        return false;
    }

    if (!pb_read(stream, buffer, stream->bytes_left)) {
        Serial.println("Failed2");
        return false;
    }
    switch (check) {
        case 0:
            msg_id = (const char *) buffer;
            check++;
            break;
        case 1:
            resource = (const char *) buffer;
            check++;
            break;
        case 2:
            value = (const char *) buffer;
            check = 0;
            break;
        default:
            check = 0;
            break;
    }
    return true;

};


class iot_msg_decode : public msg_enums {


public:

    iot_msg_decode(char *_incoming_buffer, size_t available_bytes) : type(NONE) {
        reset_values();
        pb_istream_t stream = pb_istream_from_buffer((uint8_t *) _incoming_buffer, available_bytes);

        _msg.resource.funcs.decode = &decode_msg_;
        _msg.msg_id.funcs.decode = &decode_msg_;

        bool status = pb_decode(&stream, Message_fields, &_msg);;

        if (!status) {
            Serial.print("Decoding failed: ");
            Serial.println(PB_GET_ERROR(&stream));
        } else {
            Serial.println("Decoding success");
        }

    }

    ~iot_msg_decode() {
        reset_values();
    }

    void reset_values(){
        msg_id = "";
        resource = "";
        value = "";
        check = 0;
    }


    String get_msg_id() {
        return msg_id;
    }

    iot_msg_types get_msg_type() {
        return type;
    }

    String get_resources() {
        return resource;
    }

    String get_value() {
        return value;
    }




private:

    iot_msg_types type;
    Message _msg = Message_init_zero;
};