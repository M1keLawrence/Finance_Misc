#ifndef NODE_CPP
#define NODE_CPP

#include "Node.hpp"
#include <stdexcept>

template<typename T>
Node<T>::Node(const T& value) : data(value), next(nullptr) {}

#endif 