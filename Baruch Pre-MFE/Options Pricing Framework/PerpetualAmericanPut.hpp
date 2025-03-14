#ifndef PerpetualAmericanPut_hpp
#define PerpetualAmericanPut_hpp
#include "OptionBase.hpp"
using namespace std;

namespace MikeLawrence
{
	namespace Pricer
	{
		class PerpetualAmericanPut : OptionBase
		{
		public:

			PerpetualAmericanPut(); // default constructor
			PerpetualAmericanPut(const double r, const double sig, const double K, const double b);
			PerpetualAmericanPut(const PerpetualAmericanPut& amerOpt); // copy constructor
			~PerpetualAmericanPut();

			double Price(const double U) const;

			friend ostream& operator << (ostream& os, const PerpetualAmericanPut& source); // ostream operator
		};
	} // end of namespace Pricer
} // end of namespace MikeLawrence
#endif