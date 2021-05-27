#include "../protobuf/pb_common.h"
#include "../protobuf/message.pb.h"
#include "../protobuf/pb.h"
#include "../protobuf/pb_encode.h"
#include "../protobuf/pb_decode.h"
//#include "./iot_enums.hpp"

bool encode_string(pb_ostream_t *stream, const pb_field_t *field, void *const *arg) {
    const char *str = (const char *) (*arg);

    if (!pb_encode_tag_for_field(stream, field))
        return false;

    return pb_encode_string(stream, (uint8_t *) str, strlen(str));
}

class iot_msg_encode : public msg_enums {

public:
    iot_msg_encode(const char *msg_id, iot_msg_types msg_type, const char *resource, const char *value) :
            msg_id_(msg_id), msg_type_(msg_type), resource_(resource), value_(value) {}

    ~iot_msg_encode() {

    }

    Message encode() {
        Message msg = Message_init_zero;

        msg.msg_id.arg = (void *) msg_id_;
        msg.msg_id.funcs.encode = &encode_string;

        msg.msg_type = msg_type_;

        msg.resource.arg = (void *) resource_;
        msg.resource.funcs.encode = &encode_string;

        msg.value.arg = (void *) value_;
        msg.value.funcs.encode = &encode_string;

        return msg;
    }


private:
    const char *msg_id_;
    iot_msg_types msg_type_;
    const char *resource_;
    const char *value_;
};