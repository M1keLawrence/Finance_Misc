#ifndef LISTITERATOR_HPP
#define LISTITERATOR_HPP

#include "Node.hpp"

template <typename T>
class ListIterator {
private:
    Node<T>* current;

public:
    ListIterator(Node<T>* start);

    bool HasNext();

    T& Next();
};

#ifndef LISTITERATOR_CPP // Allows for separation of declaration and implementation for the class
#include "ListIterator.cpp"
#endif

#endif
