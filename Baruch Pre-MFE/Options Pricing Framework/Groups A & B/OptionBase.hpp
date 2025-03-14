#ifndef OptionPricer_hpp
#define OptionPricer_hpp
#include <string>
#include "boost/variant.hpp"
using namespace std;


namespace MikeLawrence
{
	namespace Pricer
	{
		class OptionBase
		{
		private:

			double id;		// id
			double r;		// Interest rate
			double sig;		// Volatility 
			double k;		// Strike price 
			double t;		// Expiry date 
			double b;		// Cost of carry  

		public:
			OptionBase(); //default constructor
			OptionBase(const double& r, const double& sig, const double& K, const double& T, const double& b); // input constructor
			OptionBase(const OptionBase& optBase_input); // copy consturctor
			~OptionBase(); // destructor

			double R() const; // getters that cannot edit the parameters
			double Sig() const;
			double K() const;
			double T() const;
			double B() const;
			double ID() const; 

			void R(const double& R); // setters
			void Sig(const double& Sig);
			void K(const double& K);
			void T(const double& T);
			void B(const double& B);
			void ID(const double& ID);

			OptionBase& operator= (const OptionBase& source); // assignment operator

		};
	}
}
#endif