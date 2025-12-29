#ifndef LISTITERATOR_CPP
#define LISTITERATOR_CPP

#include "ListIterator.hpp"
#include <stdexcept>

template<typename T>
ListIterator<T>::ListIterator(Node<T>* start) : current(start) {}

template<typename T>
bool ListIterator<T>::HasNext() {
    return current != nullptr;
}

template<typename T>
T& ListIterator<T>::Next() {
    T& value = current->data;
    current = current->next;
    return value;
}

#endif