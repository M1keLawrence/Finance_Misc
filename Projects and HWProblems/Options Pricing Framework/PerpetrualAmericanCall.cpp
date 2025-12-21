#include "PerpetualAmericanCall.hpp"
#include <cmath>
#include <iostream>

namespace MikeLawrence
{
	namespace Pricer
	{
		// default constructor
		PerpetualAmericanCall::PerpetualAmericanCall() : OptionBase::OptionBase() {} 

		// input constructor, allows user to specify each parameter
		PerpetualAmericanCall::PerpetualAmericanCall(const double r, const double sig, const double K, const double b)
			: OptionBase::OptionBase(r, sig, K, 0.0, b) {} 
		// once again, a slightly ungly hard coded solution but I can't think of an error this would cause. 

		// copy constructor
		PerpetualAmericanCall::PerpetualAmericanCall(const PerpetualAmericanCall& perpetAmerCall) : OptionBase::OptionBase(perpetAmerCall) {}

		PerpetualAmericanCall::~PerpetualAmericanCall() {}
		
		// return price of the perpetual american call
		double PerpetualAmericanCall::Price(const double U) const
		{//  y1 = 0.5 - (b/sig^2) + sqrt( (b/sig^2)^2 +2*r/sig^2 ) 
			double temp2 = (this->B() / pow(this->Sig(), 2)) - 0.5;
			double y1 = 0.5 - (this->B() / pow(this->Sig(), 2)) +
				sqrt(pow(temp2, 2) + ((2 * this->R()) / pow(this->Sig(), 2))); // end of sqrt()

			double tmp = ((y1 - 1) * U) / (y1 * this->K());
			double C = (this->K() / (y1 - 1)) * pow(tmp, y1);
			return C;
		}

		ostream& operator << (ostream& os, const PerpetualAmericanCall& source) // ostream operator 
		{
			os << endl;
			os << "EuroPut: \n" <<
				"ID: " << source.ID() << "\n" <<
				"R: " << source.R() << "\n" <<
				"Sig: " << source.Sig() << "\n" <<
				"K: " << source.K() << "\n" <<
				"T: " << source.T() << "\n" <<
				"B: " << source.B() << "\n" << endl; // output a description of the Put

			return os; // return the description to be printed. 
		}
	} // end of namespace Pricer
} // end of namespace Pricer
