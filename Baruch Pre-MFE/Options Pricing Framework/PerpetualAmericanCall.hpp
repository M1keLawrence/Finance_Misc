#ifndef PerpetualAmericanCall_hpp
#define PerpetualAmericanCall_hpp
#include "OptionBase.hpp"
using namespace std;

namespace MikeLawrence
{
	namespace Pricer 
	{
		class PerpetualAmericanCall : OptionBase
		{
		public:

			PerpetualAmericanCall(); // default constructor
			PerpetualAmericanCall(const double r, const double sig, const double K, const double b);
			PerpetualAmericanCall(const PerpetualAmericanCall& amerOpt); // copy constructor
			~PerpetualAmericanCall();

			double Price(const double U) const; 

			friend ostream& operator << (ostream& os, const PerpetualAmericanCall& source); // ostream operator
		};
	} // end of namespace Pricer
} // end of namespace MikeLawrence
#endif