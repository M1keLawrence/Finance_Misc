#ifndef DOUBLYLINKEDLIST_HPP
#define DOUBLYLINKEDLIST_HPP

#include "AbstractLinkedList.hpp"

template <typename T>
class DNode : public Node<T> {
public:
    DNode<T>* prev;

    DNode(const T& value)
        : Node<T>(value), prev(nullptr) {}
};

/****************************************
 * DoublyLinkedList
 ****************************************/
template <typename T>
class DoublyLinkedList : public LinkedList<T> {
private:
    DNode<T>* head;
    DNode<T>* tail;
    int size;

    // Only traverses through Node::next, which is fine because DNode inherits it
    DNode<T>* GetNode(int index) {
        if (index < 0 || index >= size)
            throw std::out_of_range("Index out of range");

        DNode<T>* cur;

        if (index < size / 2) {
            cur = head;
            for (int i = 0; i < index; i++)
                cur = static_cast<DNode<T>*>(cur->next);
        }
        else {
            cur = tail;
            for (int i = size - 1; i > index; i--)
                cur = cur->prev;
        }
        return cur;
    }

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), size(0) {}

    /**************** Add(value) ****************/
    void Add(const T& value) override {
        DNode<T>* node = new DNode<T>(value);

        if (!head) {
            head = tail = node;
        }
        else {
            tail->next = node;         // Node::next works
            node->prev = tail;         // DNode::prev works
            tail = node;
        }
        size++;
    }

    /**************** Insert(value, index) ****************/
    void Insert(const T& value, int index) override {
        if (index < 0 || index > size)
            throw std::out_of_range("Invalid index");

        if (index == size) {
            Add(value);
            return;
        }

        DNode<T>* node = new DNode<T>(value);

        if (index == 0) {
            node->next = head;
            head->prev = node;
            head = node;
        }
        else {
            DNode<T>* cur = GetNode(index);
            DNode<T>* prevNode = cur->prev;

            node->next = cur;
            node->prev = prevNode;
            prevNode->next = node;
            cur->prev = node;
        }
        size++;
    }

    /**************** Get(index) ****************/
    T& Get(int index) override {
        return GetNode(index)->data;
    }

    /**************** Remove(index) ****************/
    T& Remove(int index) override {
        DNode<T>* node = GetNode(index);
        T& val = node->data;  // must return reference to data

        if (node == head && node == tail) {
            head = tail = nullptr;
        }
        else if (node == head) {
            head = static_cast<DNode<T>*>(head->next);
            head->prev = nullptr;
        }
        else if (node == tail) {
            tail = tail->prev;
            tail->next = nullptr;
        }
        else {
            node->prev->next = node->next;
            static_cast<DNode<T>*>(node->next)->prev = node->prev;
        }

        // ✔️ DO NOT DELETE node
        // because Remove must return T& and caller must ensure value outlives list
        size--;

        return val;
    }

    int Size() const override {
        return size;
    }

    int IndexOf(const T& value) override {
        DNode<T>* cur = head;
        int idx = 0;
        while (cur) {
            if (cur->data == value) return idx;
            cur = static_cast<DNode<T>*>(cur->next);
            idx++;
        }
        return -1;
    }

    ListIterator<T> Iterator() override {
        return ListIterator<T>(head);
    }
};

#endif
