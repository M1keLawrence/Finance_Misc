#ifndef EXCEPTION_HPP
#define	EXCEPTION_HPP
#include <string>
#include <iostream>
#include <sstream>
using namespace std;

class ArrayException // create the AE class 
{
private:

public:
	ArrayException() {} // create constructor and destructor
	~ArrayException() {}

	virtual string GetMessage() const = 0; // create the PVMF for returning the error message 
};

#endif