#ifndef IOT_MAP_H
#define IOT_MAP_H

#include <string.h>

template <class T>
class iot_map {

public:
    iot_map() : head_(NULL), last_(NULL) {

    }

    virtual ~iot_map() {
    }

public:

    struct entry {
        entry(const char* key) : key_(key), next_(NULL){

        }

        const char* key_;
        struct entry * next_;
        T value_;
    };

private:

    entry * head_;
    entry * last_;

public:

    T& operator[](const char* key){
        entry * current = head_;
        while(current != NULL){
            if(strcmp(key, current->key_)==0){
                return current->value_;
            }
            current = current->next_;
        }
        // TODO replace with memory allocator for allowing static memory/dynamic memory
        current = new entry(key);

        if(head_==NULL) head_ = current;
        if(last_!=NULL) last_->next_ = current;
        last_ = current;
        return current->value_;
    }

    entry* begin(){
        return head_;
    }

    entry* end(){
        return last_;
    }

    bool empty()
    {
        return head_ == last_;
    }

    T* find(const char* key)
    {
        if(key==NULL) return NULL;
        entry * current = head_;
        while(current != NULL){
            if(strcmp(key, current->key_)==0){
                return &current->value_;
            }
            current = current->next_;
        }
        return NULL;
    }

};

#endif