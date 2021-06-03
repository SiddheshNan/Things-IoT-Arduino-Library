
#include "../ArduinoJson.h"
#include "./iot_map.hpp"


enum iot_io_type {
    none = 0,

    run = 1, // only callback
    data_out = 2, // only view from cloud
    data_io = 3 // control from both sides
};


struct Resource {


    const char *resource_name;

    iot_io_type io_type = none;

    union {
        void (*run)();

        void (*data_out)(JsonDocument &out);

        void (*data_io)(JsonDocument &in, JsonDocument &out);
    };

};


class iot_resource {
public:
    const char *tmp = "";

    iot_resource &operator[](const char *key) {
        tmp = key;
        return *this;
    }

    //------------------


    // operator = only if data in AND data out
    void operator=(void (*run_function)(JsonDocument &in, JsonDocument &out)) {
        Resource _res;
        _res.resource_name = tmp;
        _res.io_type = iot_io_type::data_io;
        _res.data_io = run_function;
        resource_map[tmp] = _res;
        tmp = "";
    };

    // run function, with only callback to run
    void operator=(void (*run_function)()) {
        Resource _res;
        _res.resource_name = tmp;
        _res.io_type = iot_io_type::run;
        _res.run = run_function;
        resource_map[tmp] = _res;
        tmp = "";
    };


    // data out
    void operator<<(void (*run_function)(JsonDocument &out)) {
        Resource _res;
        _res.resource_name = tmp;
        _res.io_type = iot_io_type::data_out;
        _res.data_out = run_function;
        resource_map[tmp] = _res;
        tmp = "";
    };


    void fill_api_resources(JsonDocument &data_out) {

        iot_map<Resource>::entry *current = resource_map.begin();

        while (current != NULL) {
            data_out["out"][current->key_] = current->value_.io_type;
            current = current->next_;
        }


    }

    void run_callback_data_out(JsonDocument &in, JsonDocument &out) {

        String in_resource = in["res"];
        iot_map<Resource>::entry *current = resource_map.begin();


        while (current != NULL) {

            if (in_resource == current->key_) {


                switch (current->value_.io_type) {
                    case iot_io_type::run : {
                        current->value_.run();
                    }
                        break;

                    case iot_io_type::data_out: {
                        current->value_.data_out(out);
                    }

                        break;

                    case iot_io_type::data_io: {
                        StaticJsonDocument<8> _in;
                        current->value_.data_io(_in, out);
                    }
                        break;

                    default:
                        break;

                }
                break;
            }
            current = current->next_;
        }
    }


    void run_callback_data_io(JsonDocument &in, JsonDocument &out) {

        String in_resource = in["res"];

        iot_map<Resource>::entry *current = resource_map.begin();
        while (current != NULL) {

            if (in_resource == current->key_) {


                switch (current->value_.io_type) {

                    case iot_io_type::run : {
                        current->value_.run();
                    }

                        break;

                    case iot_io_type::data_out: {
                        current->value_.data_out(out);
                    }
                        break;

                    case iot_io_type::data_io: {
                        StaticJsonDocument<200> _in;
                        _in = in["in"];
                        current->value_.data_io(_in, out);
                    }

                        break;

                    default:
                        break;

                }
                break;
            }
            current = current->next_;
        }
    }


    void run() {

    }


private:

    iot_map<Resource> resource_map;
};