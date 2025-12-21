#include "EuroCall.hpp"
#include <boost/math/distributions/normal.hpp>
#include <boost/math/distributions.hpp> 
#include <iostream>
using namespace boost::math;
using namespace std;

namespace MikeLawrence
{
	namespace Pricer
	{
		//default constructor
		EuroCall::EuroCall() : OptionBase::OptionBase() {}

		// copy constructor
		EuroCall::EuroCall(const EuroCall& o2) : OptionBase::OptionBase(o2) {}

		// input constructor, allows user to specify each parameter
		EuroCall::EuroCall(const double r, const double sig, const double K, const double T, const double b)
			: OptionBase::OptionBase(r, sig, K, T, b){} 

		EuroCall::~EuroCall() {} // destructor

		double EuroCall::Price(const double U) const
		{ // return the price of the call option at underlying price U
			normal_distribution<> myNormal;

			double tmp = this->Sig() * sqrt(this->T());

			double d1 = (log(U / this->K()) + (this->B() + (this->Sig() * this->Sig()) * 0.5) * this->T()) / tmp;

			double d2 = d1 - tmp;

			return (U * exp((this->B() - this->R()) * this->T()) * cdf(myNormal, d1)) - (this->K() * exp(-(this->R()) * this->T()) * cdf(myNormal, d2));

		}

		double EuroCall::Delta(const double U) const
		{ // return the delta of the call option at underlying price U
			normal_distribution<> myNormal;

			double tmp = this->Sig() * sqrt(this->T());

			double d1 = (log(U / this->K()) + (this->B() + (this->Sig() * this->Sig()) * 0.5) * this->T()) / tmp;

			return exp((this->B() - this->R()) * this->T()) * cdf(myNormal, d1);
		}

		double EuroCall::DivDiff_Delta(const double U, const double h) const
		{
			double numerator = ((*this).Price(U + h) - (*this).Price(U - h)); 
			if (abs(numerator) <= pow(2, -53)) // a very helpful thread on QN forum led me to read up on machine epsilons.  
			{ // the cutoff for doubles in cpp is 2^-53. I almost didn't include 2^-53, and left it at less than, which would have been a shame. 
				cout << "H is too small for accurate computation." << endl; // acount for machine limitations
				return 0.0;
			}
			else
			{
				return numerator / (2 * h); // If machine limitations are not run against, return the result.

			}
		}

		double EuroCall::Gamma(const double U) const
		{// calculate and returnt the gamma of the put at underlying price U
			normal_distribution<> myNormal;

			double tmp = this->Sig() * sqrt(this->T());

			double d1 = (log(U / this->K()) + (this->B() + (this->Sig() * this->Sig()) * 0.5) * this->T()) / tmp;

			return (pdf(myNormal, d1) * exp((this->B() - this->R()) * this->T())) / (U * this->Sig() * sqrt(this->T()));
		}

		double EuroCall::DivDiff_Gamma(const double U, const double h) const
		{ // return the Gamma of the call at underlying U using the Divided Difference method and a difference of h. 

			// calculate numerator value, for cheking and solution
			double numerator = ((*this).Price(U + h) - 2 * (*this).Price(U) + (*this).Price(U - h)); 

			if (abs(numerator) <= pow(2, -53)) 
			{
				cout << "H is too small for accurate computation." << endl; // acount for machine limitations
				return 0.0;
			}
			else
			{
				return numerator / (h * h); // If machine limitations are not run against, return the result. 
			}
		}

		double EuroCall::PutCallParity(double U) const
		{ // return the price of an equivalent put at underlying price U
			double P;
			P = (*this).Price(U) + (this->K() * exp((-(this->R())) * this->T())) - U;
			return P;
		}

		EuroCall& EuroCall::operator= (const EuroCall& source) // assignment operator
		{
			if (this == &source)
				return *this;

			OptionBase::operator = (source); // copy the member data from the OptionsBase class

			return (*this);
		}

		ostream& operator << (ostream& os, const EuroCall& source) // ostream operator
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
	}
}