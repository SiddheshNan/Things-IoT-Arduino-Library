class msg_enums {
public:

    enum iot_msg_fields {
        MSG_TYPE = 1,
        MSG_ID = 2,
        RESOURCE = 3,
        VALUE = 4
    };

    enum iot_msg_types {
        NONE = 0,
        OK = 1,
        ERROR = 2,
        CONNECT = 3, /// AUTH
        DISCONNECT = 4,
        KEEP_ALIVE = 5,


        RESOURCE_GET_VAL = 6,
        RESOURCE_SET_VAL = 7,
        RESOURCE_RESPONSE = 8,
        RESOURCE_DESCRIBE = 9,


        STREAM_START = 10,
        STREAM_STOP = 11,
        STREAM_SAMPLE = 12,
        STREAM_RESPONSE = 13,
        STREAM_KEEP_ALIVE = 14

        // TODO: MORE FEATURES
//
//            CALL_ENDPOINT       = 8,    // call the endpoint with the provided name (endpoint_id in identifier, value passed in payload)
//            CALL_DEVICE         = 9,    // call a given device (device_id in identifier, resource in resource, and value, passed in payload)
//            BUCKET_DATA         = 10,    // call the bucket with the provided name (bucket_id in identifier, value passed in payload)
//            GET_PROPERTY        = 11,    // call the bucket with the provided name (bucket_id in identifier, value passed in payload)
//            SET_PROPERTY        = 12    // call the bucket with the provided name (bucket_id in identifier, value passed in payload)
    };
};