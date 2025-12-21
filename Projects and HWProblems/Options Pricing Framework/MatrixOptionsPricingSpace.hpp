#ifndef MatrixOptionsPricingSpace_hpp
#define MatrixOptionsPricingSpace_hpp
#include <vector>
#include <map>
#include <iostream>
#include "EuroCall.hpp"
#include "EuroPut.hpp"
using namespace std;
/*
	Thoughts: 
		Debating spliting this into multiple classes, but not sure I want to go through with it. 
*/

namespace MikeLawrence
{
	namespace Pricer
	{

		
		class MatrixOptionsPricingSpace // at the very end test if this all works as a template class as well. 
		{
		private:

			string d_mem_option_type;  
			vector<double> d_mem_vec_r;
			vector<double> d_mem_vec_sig;
			vector<double> d_mem_vec_K;
			vector<double> d_mem_vec_T;
			vector<double> d_mem_vec_b;
			vector<double> d_mem_vec_S;
			vector<double> d_mem_vector_of_prices; // self contained results collector, for easy calling and use of user after original pricing
			vector<double> d_mem_vector_of_deltas;
			vector<double> d_mem_vector_of_gammas;


		public: 
			MatrixOptionsPricingSpace(); // default constructor

			MatrixOptionsPricingSpace(const vector<vector<double>>& matrix_of_inputs, const string& opt_type); // matrix input consturctor

			MatrixOptionsPricingSpace(const vector<double>& vec_r_input, const vector<double>& vec_sig_input, const vector<double>& vec_K_input,
								const vector<double>& vec_T_input, const vector<double>& vec_b_input, const vector<double>& vec_S_input,
								const string& opt_type); // by hand input constructor

			MatrixOptionsPricingSpace(const MatrixOptionsPricingSpace& copy); // copy constructor 

			~MatrixOptionsPricingSpace(); // destructor 

			vector<double> OptionPricingMatrix(); // return a vector of option prices based on a matrix of parameter inputs

			vector<double> GreekSolverMatrix(const string& greek_type); // return a vector of greeks based on a matrix of parameter inputs

			void Print() const; // print a vector (often a vector output from a member function)
			void Print(const string& identifier) const; // other form of print function, for when one print is needed and known upon call. 

			const vector<double>& vector_of_prices() const; // return the parameter's vector of values; none work as setters.
			const vector<double>& vector_of_deltas() const;
			const vector<double>& vector_of_gammas() const;
			const vector<double>& r_vec() const; 
			const vector<double>& sig_vec() const;
			const vector<double>& K_vec() const;
			const vector<double>& T_vec() const;
			const vector<double>& b_vec() const;
			const vector<double>& S_vec() const; 
			const string& option_type() const;

			void vector_of_prices(const vector<double>& vecPrices); // set parameters
			void vector_of_deltas(const vector<double>& vecDelt);
			void vector_of_gammas(const vector<double>& vecGamma);
			void r_vec(const vector<double>& vecR); 
			void sig_vec(const vector<double>& vecSig);
			void K_vec(const vector<double>& vecK);
			void T_vec(const vector<double>& vecT);
			void b_vec(const vector<double>& vecB); 
			void S_vec(const vector<double>& vecS);
			void option_type(const string& optType); 

		};
	}
}
#endif