#ifndef EuroPut_hpp 
#define EuroPut_hpp 
#include "OptionBase.hpp"

namespace MikeLawrence
{
	namespace Pricer
	{
		class EuroPut : public OptionBase
		{
		public:
			EuroPut();							// Default option
			EuroPut(const EuroPut& option2);	// Copy constructor
			EuroPut(const double r, const double sig, const double K, const double T, const double b); // flexible constructor
			~EuroPut();

			// overload the pvmf of the base class
			// necessary, while also prividing functionality to the derived class
			double Price(const double U) const; // return the price of the put
			double Delta(const double U) const; // return the delta of the put
			double DivDiff_Delta(const double U, const double h) const; // retur the delta of the put by use of divided difference methods
			double DivDiff_Gamma(const double U, const double h) const; // retur the delta of the put by use of divided difference methods
			double Gamma(const double U) const; // return the gamma of the put
			

			double PutCallParity(const double U) const; // return the price of an equivalent call at underlying price 

			EuroPut& operator= (const EuroPut& source); // assignment operator

			friend ostream& operator << (ostream& os, const EuroPut& source); // ostream operator 
		};
	}
}
#endif