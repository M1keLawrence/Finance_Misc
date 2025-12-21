#include "OptionBase.hpp"

namespace MikeLawrence
{
	namespace Pricer
	{
		//default constructor
		OptionBase::OptionBase() : r(0.0), sig(0.0), k(0.0), t(0.0), b(0.0), id(rand()) {}

		// copy constructor
		OptionBase::OptionBase(const OptionBase& o2) : r(o2.r), sig(o2.sig), k(o2.k), t(o2.t), b(o2.b), id(o2.id) {}

		// input constructor, allows user to specify each parameter
		OptionBase::OptionBase(const double& r, const double& sig, const double& K, const double& T, const double& B) 
								: r(r), sig(sig), k(K), t(T), b(B), id(rand()) {} 
		// you can set the id if you want using a member function, but this is not necessary so not required upon construction. 

		OptionBase::~OptionBase() {} // destructor

		double OptionBase::R() const // get r; works as setter; return type must be const to avoid reading errors in compiling
		{
			return r;
		}

		double OptionBase::Sig() const // get sig; works as setter; return type must be const to avoid reading errors in compiling
		{
			return sig;
		}

		double OptionBase::K() const // get K; works as setter; return type must be const to avoid reading errors in compiling 
		{
			return k;
		}

		double OptionBase::T() const // get T; works as setter; return type must be const to avoid reading errors in compiling 
		{
			return t;
		}

		double OptionBase::B() const // get b; works as setter; return type must be const to avoid reading errors in compiling
		{
			return b;
		}

		double OptionBase::ID() const
		{
			return id;
		}

		void OptionBase::R(const double& R)  // get r; modifyable for future purposes
		{
			r = R;
		}

		void OptionBase::Sig(const double& Sig)  // get sig; modifyable for future purposes
		{
			sig = Sig;
		}

		void OptionBase::K(const double& K_in)  // get K; modifyable for future purposes
		{
			k = K_in;
		}

		void OptionBase::T(const double& T)  // get T; modifyable for future purposes
		{
			 t = T;
		}

		void OptionBase::B(const double& B)  // get b; modifyable for future purposes
		{
			b = B;
		}

		void OptionBase::ID(const double& ID)
		{
			id = ID; 
		}

		OptionBase& OptionBase::operator= (const OptionBase& source)
		{
			if (this == &source)
				return *this;

			r = source.r;
			sig = source.sig;
			k = source.k;
			t = source.t;
			b = source.b;

			return *this;
		}
	}
}