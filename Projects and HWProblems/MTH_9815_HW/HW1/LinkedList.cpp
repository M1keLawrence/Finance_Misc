#ifndef LINKEDLIST_CPP
#define LINKEDLIST_CPP

#include "LinkedList.hpp"
#include <stdexcept>

// Constructor
template <typename T>
LinkedList<T>::LinkedList() : head(nullptr), size(0) {}

// Destructor - only destructor implemented becasue Node and ListIterator don't 
//              own any data but just point to data and so delete themselves
template <typename T>
LinkedList<T>::~LinkedList() {
    Node<T>* curr = head;
    while (curr) {
        Node<T>* next = curr->next;
        delete curr;
        curr = next;
    }
}

template <typename T>
void LinkedList<T>::Add(const T& value) {
    Insert(value, size);
}

template <typename T>
void LinkedList<T>::Insert(const T& value, int index) {
    if (index < 0 || index > size)
        throw std::out_of_range("Index out of range");

    Node<T>* newNode = new Node<T>(value);

    if (index == 0) {
        newNode->next = head;
        head = newNode;
    } else {
        Node<T>* prev = GetNode(index - 1);
        newNode->next = prev->next;
        prev->next = newNode;
    }

    size++;
}

template <typename T>
T& LinkedList<T>::Get(int index) {
    return GetNode(index)->data;
}

template <typename T>
Node<T>* LinkedList<T>::GetNode(int index) {
    if (index < 0 || index >= size)
        throw std::out_of_range("Index out of range");

    Node<T>* curr = head;
    for (int i = 0; i < index; i++)
        curr = curr->next;

    return curr;
}

template <typename T>
int LinkedList<T>::IndexOf(const T& value) {
    Node<T>* curr = head;
    int idx = 0;

    while (curr) {
        if (curr->data == value)
            return idx;

        curr = curr->next;
        idx++;
    }

    return -1;
}

template <typename T>
T& LinkedList<T>::Remove(int index) {
    if (index < 0 || index >= size)
        throw std::out_of_range("Index out of range");

    Node<T>* removed;

    if (index == 0) {
        removed = head;
        head = head->next;
    } else {
        Node<T>* prev = GetNode(index - 1);
        removed = prev->next;
        prev->next = removed->next;
    }
    //delete removed; - can't do this because homework specifies that we need to return a reference, so deletion must be responsibility of function caller
    size--;

    return removed->data;
}

template <typename T>
ListIterator<T> LinkedList<T>::Iterator() {
    return ListIterator<T>(head);
}

template <typename T>
int LinkedList<T>::Size() const {
    return size;
}

#endif
