#ifndef NODE_HPP
#define NODE_HPP

template <typename T>
class Node {
public:
    T data;
    Node<T>* next;

    Node(const T& value);
};

#ifndef NODE_CPP // Allows for separation of declaration and implementation for the class
#include "Node.cpp"
#endif

#endif
