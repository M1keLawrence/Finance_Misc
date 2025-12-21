#include "EuroPut.hpp"
#include <boost/math/distributions/normal.hpp>
#include <boost/math/distributions.hpp>
#include <iostream>
#include <cmath>
using namespace boost::math;
using namespace std;

namespace MikeLawrence
{
	namespace Pricer
	{
		//default constructor
		EuroPut::EuroPut() : OptionBase::OptionBase() {}

		// copy constructor
		EuroPut::EuroPut(const EuroPut& o2) : OptionBase::OptionBase(o2) {}

		// input constructor, allows user to specify each parameter
		EuroPut::EuroPut(const double r, const double sig, const double K, const double T, const double b)
							 : OptionBase::OptionBase(r, sig, K, T, b) {}

		EuroPut::~EuroPut() {}

		double EuroPut::Price(const double U) const
		{// return the price of the put option at underlying price U
			normal_distribution<> myNormal;

			double tmp = this->Sig() * sqrt(this->T());
			double d1 = (log(U / this->K()) + (this->B() + (this->Sig() * this->Sig()) * 0.5) * this->T()) / tmp;
			double d2 = d1 - tmp;

			return (this->K() * exp(-this->R() * this->T()) * cdf(myNormal, -d2)) - (U * exp((this->B() - this->R()) * this->T()) * cdf(myNormal, -d1));

		}

		double EuroPut::Delta(const double U) const
		{// return the delta of the put option at underlying price U
			normal_distribution<> myNormal;
			double tmp = this->Sig() * sqrt(this->T());
			double d1 = (log(U / this->K()) + (this->B() + (this->Sig() * this->Sig()) * 0.5) * this->T()) / tmp;

			return exp((this->B() - this->R()) * this->T()) * (cdf(myNormal, d1) - 1.0);
		}

		double EuroPut::DivDiff_Delta(const double U, const double h) const
		{
			double numerator = ((*this).Price(U + h) - (*this).Price(U - h));
			if (abs(numerator) <= pow(2, -53)) // a very helpful thread on QN forum led me to read up on machine epsilons. 
			{ // the cutoff for doubles in cpp is 2^-53. I almost didn't include 2^-53, and left it at less than, which would have been a shame. 
				cout << "H is too small for accurate computation." << endl;
				return 0.0;
			}
			else
			{
				return numerator / (2 * h);

			}
		}

		double EuroPut::Gamma(const double U) const
		{// calculate and returnt the gamme of the put at underlying price U
			normal_distribution<> myNormal;
			double tmp = this->Sig() * sqrt(this->T());
			double d1 = (log(U / this->K()) + (this->B() + (this->Sig() * this->Sig()) * 0.5) * this->T()) / tmp;

			return (pdf(myNormal, d1) * exp((this->B() - this->R()) * this->T())) / (U * this->Sig() * sqrt(this->T()));
		}

		double EuroPut::DivDiff_Gamma(const double U, const double h) const
		{
			double numerator = ((*this).Price(U + h) - 2 * (*this).Price(U) + (*this).Price(U - h));
			if (abs(numerator) <= pow(2, -53))
			{
				cout << "H is too small for accurate computation." << endl;
				return 0.0;
			}
			else
			{
				return numerator / (h * h);
			}
		}

		double EuroPut::PutCallParity(const double U) const
		{ // return the price of an equivalent call option at underlying price U
			double C; 
			C = (*this).Price(U) + U - (this->K() * exp((-this->R()) * this->T()));
			return C;
		}

		EuroPut& EuroPut::operator= (const EuroPut& source)
		{
			if (this == &source)
				return *this;

			OptionBase::operator = (source);

			return (*this);
		}

		ostream& operator << (ostream& os, const EuroPut& source) 
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
} // end of namespace MikeLawrence