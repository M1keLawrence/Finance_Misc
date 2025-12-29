#ifndef ABSTRACTLINKEDLIST_HPP
#define ABSTRACTLINKEDLIST_HPP

#include <iostream>
#include <stdexcept>
#include "ListIterator.hpp"
using namespace std;


/****************************************
 * LinkedList Interface
 ****************************************/
template <typename T>
class AbstractLinkedList {
public:
    virtual void Add(const T& value) = 0;
    virtual void Insert(const T& value, int index) = 0;
    virtual T& Get(int index) = 0;
    virtual int IndexOf(const T& value) = 0;
    virtual T& Remove(int index) = 0;
    virtual ListIterator<T> Iterator() = 0;
    virtual int Size() const = 0;

    virtual ~AbstractLinkedList() {}
};

#endif


