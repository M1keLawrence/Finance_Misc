#ifndef SizeMismatchException_HPP
#define SizeMismatchException_HPP
#include "ArrayException.hpp"
#include <string>
#include <sstream>
using namespace std;

class SizeMismatchException : public ArrayException
{
public:
	SizeMismatchException() : ArrayException() {} // default constructor
	~SizeMismatchException() {}  // destructor

	string GetMessage() const // override the PVMF to return the error message
	{
		stringstream ss;
		ss << "The Array's are of different size.";
		return ss.str();
	}

};

#endif