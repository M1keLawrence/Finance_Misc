#include "PerpetualAmericanPut.hpp"
#include <cmath>
#include <iostream>

namespace MikeLawrence
{
	namespace Pricer
	{
		// default constructor
		PerpetualAmericanPut::PerpetualAmericanPut() : OptionBase::OptionBase() {}

		// input constructor, allows user to specify each parameter
		PerpetualAmericanPut::PerpetualAmericanPut(const double r, const double sig, const double K, const double b)
			: OptionBase::OptionBase(r, sig, K, 0.0, b) {} // this feels like cheating and might not be ok but I can't think of a 
															// good enough reason to not do it so I'm going with it. It won't be used, just a little ugly.

		// copy constructor
		PerpetualAmericanPut::PerpetualAmericanPut(const PerpetualAmericanPut& perpetAmerCall) : OptionBase::OptionBase(perpetAmerCall) {}

		PerpetualAmericanPut::~PerpetualAmericanPut() {}

		// return price of the perpetual american call
		double PerpetualAmericanPut::Price(const double U) const
		{//  y2 = 0.5 - (b/sig^2) - sqrt( ((b/sig^2) - 0.5)^2 +2*r/sig^2 ) 
			double temp2 = (this->B() / pow(this->Sig(), 2)) - 0.5;
			double y2 = 0.5 - (this->B() / pow(this->Sig(), 2)) -
				sqrt( pow(temp2, 2) + ((2 * this->R()) / pow(this->Sig(), 2))); // end of sqrt()

			double tmp = ((y2 - 1)* U) / (y2 * this->K());
			double C = (this->K() / (1 - y2)) * pow(tmp, y2);
			return C;
		}

		ostream& operator << (ostream& os, const PerpetualAmericanPut& source) // ostream operator  
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
