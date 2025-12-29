#ifndef LINKEDLIST_HPP
#define LINKEDLIST_HPP

#include "Node.hpp"
#include "AbstractLinkedList.hpp"
#include "ListIterator.hpp"
#include <stdexcept>

template <typename T>
class LinkedList : public AbstractLinkedList<T> {
private:
    Node<T>* head;
    int size;

public:
    LinkedList();
    ~LinkedList();

    void Add(const T& value) override;
    void Insert(const T& value, int index) override;
    T& Get(int index) override;
    int IndexOf(const T& value) override;
    T& Remove(int index) override;
    ListIterator<T> Iterator() override;
    int Size() const override;

private:
    Node<T>* GetNode(int index);
};

#ifndef LINKEDLIST_CPP // Allows for separation of declaration and implementation for the class
#include "LinkedList.cpp"
#endif

#endif
