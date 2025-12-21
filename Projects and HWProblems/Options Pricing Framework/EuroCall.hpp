
#ifndef EuroCall_hpp
#define EuroCall_hpp
#include "OptionBase.hpp"
#include <cmath>

using namespace std;

namespace MikeLawrence
{
	namespace Pricer
	{
		class EuroCall : public OptionBase 
		{
		public:
			EuroCall();							// Default option
			EuroCall(const EuroCall& option2);	// Copy constructor
			EuroCall(const double r, const double sig, const double K, const double T, const double b); // flexible constructor
			~EuroCall();

			double Price(const double U) const; // return the price of the call
			double PutCallParity(double U) const; // return the price of an equivalent put at underlying price U

			double Delta(const double U) const; // return the delta of the call
			double DivDiff_Delta(const double U, const double h) const; // return the delta of the call using divided difference method

			double Gamma(const double U) const; // return the gamme of the call
			double DivDiff_Gamma(const double U, const double h) const; // return the gamma of the call using divided difference method

			EuroCall& operator = (const EuroCall& source); // assignment operator 
			friend ostream& operator << (ostream& os, const EuroCall& source); // ostream operator 
		};
	}
}
#endif