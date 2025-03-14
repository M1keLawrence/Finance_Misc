#ifndef UnexpectedInputException_HPP
#define UnexpectedInputException_HPP 
#include "ArrayException.hpp"
#include <string>
#include <sstream>
using namespace std;

class UnexpectedInputException : public ArrayException
{
public:
	UnexpectedInputException() : ArrayException() {} // default constructor 
	~UnexpectedInputException() {}  // destructor 

	string GetMessage() const // override the PVMF to return the error message
	{
		stringstream ss;
		ss << "User has input an unexpected input.";
		return ss.str(); 
	}

};

#endif