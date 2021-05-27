#ifndef IOT_H
#define IOT_H


#include "../protobuf/pb_common.h"
#include "../protobuf/message.pb.h"
#include "../protobuf/pb.h"
#include "../protobuf/pb_encode.h"
#include "../protobuf/pb_decode.h"


namespace thingsiot {

    class thingsiot{
    public:
        thingsiot() {}

        virtual ~thingsiot() {}


        virtual void authenticate_with_server(){



        }


        void onMessage( unsigned long &currentMillis, Message &msg) {


        }

        void sendMessage(unsigned long &currentMillis){


        }


        virtual void disconnected() {

        }



    };


}


#endif