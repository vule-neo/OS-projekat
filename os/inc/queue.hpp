#ifndef PROJECT_BASE_QUEUE_HPP
#define PROJECT_BASE_QUEUE_HPP
#include "../lib/hw.h"
#include "../h/print.hpp"
#include "../h/MemoryAllocator.hpp"


template <typename T>
class Queue{
private:
    struct elem{
        T * data;
        elem * next = nullptr;
    };

    elem * head;
    elem * tail;
public:

    Queue():head(nullptr), tail(nullptr){};

    Queue(const Queue<T> &) = delete;
    Queue<T> &operator=(const Queue<T> &) = delete;

    void add(T * t);

    T * take();

    ~Queue() {
        while (head) {
            elem *old = head;
            head = head->next;
            delete old;
        }
    }
};

template<typename T>
void Queue<T>::add(T *t) {//dodaj novi na pocetak
    elem * newElem = new elem;
    newElem->data = t;
    if (!head) head = tail = newElem;
    else{
        newElem->next = head;
        head = newElem;
    }
}

template<typename T>
T *Queue<T>::take() {
    if (!head) return nullptr;// ako je prazna nullptr, a u scheduler podesi idle nit
    T *ret = tail->data;
    if (head == tail){
        delete head;
        head = tail = nullptr;
        return ret;
    }
    elem *prev = nullptr;
    for(elem * iter = head; iter != tail; prev = iter, iter = iter->next);
    //ret = tail->data;
    delete tail;
    tail = prev;
    prev->next = nullptr;
    return ret;
}



#endif //PROJECT_BASE_QUEUE_HPP
