#include "MatrixOptionsPricingSpace.hpp"
#include "EuroCall.hpp"
#include "PerpetualAmericanCall.hpp"
#include "PerpetualAmericanPut.hpp"
#include "OptionBase.hpp"
#include "SizeMismatchException.hpp"
#include "UnexpectedInputException.hpp" 
#include <vector>
#include <map>
#include <string> 
// I had included "GlobalFunctions.hpp" (must have been using it at some prior stage of testing) and got multiple definition errors
// took it out, errors gone. 
using namespace std;

namespace MikeLawrence
{
	namespace Pricer
	{
		 // implement default constructor
		MatrixOptionsPricingSpace::MatrixOptionsPricingSpace() {}

		// enable translation of matrix of inputs to a new MatrixOptionsPricer object. 
		// only works with the one global function... I might create a 'array creator' class to help force correct behavior. 
		// I'm not sure why we would want to implement a matrix when the same work of inputs to create the matrix can just be put
		// into this class and then the functionality is right there. I'll allow both methods, but I'm not sure which is better. 
		MatrixOptionsPricingSpace::MatrixOptionsPricingSpace(const vector<vector<double>>& matrix_of_inputs, const string& opt_type)
		{
			for (int i = 0; i < matrix_of_inputs.size(); i++) // matrix_of_inputs[0] has size 
			{ // for each vector in the input matrix, assign the correct element to the correct vector
			  // same problem as before, doing this with iterators is much clunkier. 
				d_mem_vec_r.push_back(matrix_of_inputs[i][0]);
				d_mem_vec_sig.push_back(matrix_of_inputs[i][1]); 
				d_mem_vec_K.push_back(matrix_of_inputs[i][2]);
				d_mem_vec_T.push_back(matrix_of_inputs[i][3]);
				d_mem_vec_b.push_back(matrix_of_inputs[i][4]);
				d_mem_vec_S.push_back(matrix_of_inputs[i][5]);
			}
			d_mem_option_type = opt_type; 
		}


		 // implement copy constructor
		MatrixOptionsPricingSpace::MatrixOptionsPricingSpace(const MatrixOptionsPricingSpace& copy) : d_mem_vec_r(copy.d_mem_vec_r), d_mem_vec_sig(copy.d_mem_vec_sig), d_mem_vec_T(copy.d_mem_vec_T),
																					d_mem_vec_K(copy.d_mem_vec_K), d_mem_vec_b(copy.d_mem_vec_b), d_mem_vec_S(copy.d_mem_vec_S),
																					d_mem_option_type(copy.d_mem_option_type),d_mem_vector_of_prices(copy.d_mem_vector_of_prices), 
																					d_mem_vector_of_deltas(copy.d_mem_vector_of_deltas), 
																					d_mem_vector_of_gammas(copy.d_mem_vector_of_gammas) {}

		// implement input constructor ; it is now saying that the [] of map_input["r"] (and the others) is not a valid operator
		// it is clearly used in the notes/examples, and in my own code before. Why would it quit on me now??
		MatrixOptionsPricingSpace::MatrixOptionsPricingSpace(const vector<double>& vec_r_input, const vector<double>& vec_sig_input, const vector<double>& vec_K_input,
												const vector<double>& vec_T_input, const vector<double>& vec_b_input, const vector<double>& vec_S_input,
												const string& opt_type) : d_mem_vec_r(vec_r_input), d_mem_vec_sig(vec_sig_input),
																		  d_mem_vec_T(vec_T_input), d_mem_vec_K(vec_K_input),
																		  d_mem_vec_b(vec_b_input), d_mem_vec_S(vec_S_input), d_mem_option_type(opt_type) {}

		 // implement destructor
		MatrixOptionsPricingSpace::~MatrixOptionsPricingSpace() {}

		vector<double> MatrixOptionsPricingSpace::OptionPricingMatrix() 
		{

			if (d_mem_vec_r.size() != d_mem_vec_sig.size() || d_mem_vec_r.size() != d_mem_vec_T.size() || d_mem_vec_r.size() != d_mem_vec_K.size() || d_mem_vec_r.size() != d_mem_vec_b.size() || d_mem_vec_r.size() != d_mem_vec_S.size())
			{
				throw SizeMismatchException(); // throw a size mismatch exception object
			}
			else
			{
				for (int i = 0; i != d_mem_vec_r.size(); i++) 
				{
					if (d_mem_option_type == "EuroCall" || d_mem_option_type == "EuroPut")
					{
						EuroCall call_tmp(d_mem_vec_r[i], d_mem_vec_sig[i], d_mem_vec_K[i], d_mem_vec_T[i], d_mem_vec_b[i]); 

						//cout << call_tmp << endl; // for debugging purposes 

						if (d_mem_option_type == "EuroCall")
						{
							d_mem_vector_of_prices.push_back(call_tmp.Price(d_mem_vec_S[i]));
						}
						else if (d_mem_option_type == "EuroPut") {
							d_mem_vector_of_prices.push_back(call_tmp.PutCallParity(d_mem_vec_S[i]));
						}
					}
					else {
						throw UnexpectedInputException(); 
					}
				}
				//(*this).Print("S"); // ============================================================================================================
				return d_mem_vector_of_prices;
			}
		}

		vector<double> MatrixOptionsPricingSpace::GreekSolverMatrix(const string& greek_type)
		{
			if (d_mem_vec_r.size() != d_mem_vec_sig.size() || d_mem_vec_r.size() != d_mem_vec_T.size() || d_mem_vec_r.size() != d_mem_vec_K.size() || d_mem_vec_r.size() != d_mem_vec_b.size() || d_mem_vec_r.size() != d_mem_vec_S.size())
			{
				throw SizeMismatchException();
			}
			else
			{
				if (greek_type == "Delta") // if delta is requested, return the correct delta 
				{

					if (d_mem_option_type == "EuroCall")
					{
						for (int i = 0; i < d_mem_vec_r.size(); i++)
						{
							EuroCall call_tmp(d_mem_vec_r[i], d_mem_vec_sig[i], d_mem_vec_K[i], d_mem_vec_T[i], d_mem_vec_b[i]);

							d_mem_vector_of_deltas.push_back(call_tmp.Delta(d_mem_vec_S[i]));
						}
						return d_mem_vector_of_deltas;
					}
					else if (d_mem_option_type == "EuroPut") 
					{
						for (int i = 0; i < d_mem_vec_r.size(); i++)
						{
							EuroPut put_tmp(d_mem_vec_r[i], d_mem_vec_sig[i], d_mem_vec_K[i], d_mem_vec_T[i], d_mem_vec_b[i]);


							d_mem_vector_of_deltas.push_back(put_tmp.Delta(d_mem_vec_S[i]));
						}
						return d_mem_vector_of_deltas;
					}
				}

				if (greek_type == "Gamma")
				{
					for (int i = 0; i < d_mem_vec_r.size(); i++)
					{
						EuroCall call_tmp(d_mem_vec_r[i], d_mem_vec_sig[i], d_mem_vec_K[i], d_mem_vec_T[i], d_mem_vec_b[i]);


						d_mem_vector_of_gammas.push_back(call_tmp.Gamma(d_mem_vec_S[i]));
					}
					return d_mem_vector_of_gammas;
				}
				else {
					throw UnexpectedInputException(); 
				}
			}
			
		}

		void MatrixOptionsPricingSpace::Print() const
		{
			int output_cnt = 0;
			string parameter_request; 
			cout << "Input the vector of member data you want returned. Options are: " 
				<< "R \n" << "Sig \n" << "K \n" << "T \n" << "B \n" 
				<< "Price \n" << "Delta \n" << "Gamma \n" << endl;
			cin >> parameter_request; 
			
			vector<double> vec_tmp; 
			vector<double>::const_iterator vec_double_const_iter; // const iter for double vectors

			if (parameter_request == "R") { vec_tmp = d_mem_vec_r; } // typed out in one line to conserve space and clarity. 

			else if (parameter_request == "Sig") { vec_tmp = d_mem_vec_sig; }   
		
			else if (parameter_request == "K") { vec_tmp = d_mem_vec_K; } 

			else if (parameter_request == "T") { vec_tmp = d_mem_vec_T; } 

			else if (parameter_request == "B") { vec_tmp = d_mem_vec_b; } 

			else if (parameter_request == "S") { vec_tmp = d_mem_vec_S; }

			else if (parameter_request == "Price") { vec_tmp = d_mem_vector_of_prices; } 

			else if (parameter_request == "Delta") { vec_tmp = d_mem_vector_of_deltas; } 

			else if (parameter_request == "Gamma") { vec_tmp = d_mem_vector_of_gammas; } 

			else { throw UnexpectedInputException();  }


			for (vec_double_const_iter = vec_tmp.begin(); vec_double_const_iter != vec_tmp.end(); ++vec_double_const_iter) // loop through and print out each element of the vector. 
			{
				cout << *vec_double_const_iter << ", "; 
				output_cnt++;
				if (output_cnt == 5)
				{ // to avoid a ridiculously long stream of outputs in either direction (down or right), I'll print in batches of 5. 
					cout << endl;
					output_cnt = 0;
				}
			}

			cout << "Would you like to print more data?\n" << "Yes or No" << endl; 
			cin >> parameter_request; 
			if (parameter_request == "Yes")
			{
				(*this).Print();
			}
		}

		void MatrixOptionsPricingSpace::Print(const string& identifier) const
		{
			int output_cnt = 0; 
			vector<double> vec_tmp;
			vector<double>::const_iterator vec_double_const_iter; // const iter for double vectors 

			if (identifier == "R") { vec_tmp = d_mem_vec_r; } // typed out in one line to conserve space and clarity. 

			else if (identifier == "Sig") { vec_tmp = d_mem_vec_sig; }

			else if (identifier == "K") { vec_tmp = d_mem_vec_K; }

			else if (identifier == "T") { vec_tmp = d_mem_vec_T; }

			else if (identifier == "B") { vec_tmp = d_mem_vec_b; } 

			else if (identifier == "S") { vec_tmp = d_mem_vec_S; }

			else if (identifier == "Price") { vec_tmp = d_mem_vector_of_prices; } 
			 
			else if (identifier == "Delta") { vec_tmp = d_mem_vector_of_deltas; } 

			else if (identifier == "Gamma") { vec_tmp = d_mem_vector_of_gammas; } 

			else { throw UnexpectedInputException(); } 

			for (vec_double_const_iter = vec_tmp.begin(); vec_double_const_iter < vec_tmp.end(); ++vec_double_const_iter) // loop through and print out each element of the vector. 
			{
				cout << *vec_double_const_iter << ", ";
				output_cnt++; 
				if (output_cnt == 5) 
				{ // to avoid a ridiculously long stream of outputs in either direction (down or right), I'll print in batches of 5. 
					cout << endl;
					output_cnt = 0;
				}
			}
		}

		const vector<double>& MatrixOptionsPricingSpace::vector_of_prices() const
		{ // get the vector of prices; no setting 
			return d_mem_vector_of_prices;
		}

		const vector<double>& MatrixOptionsPricingSpace::vector_of_deltas() const
		{ // get the vector of prices; no setting 
			return d_mem_vector_of_deltas;
		}

		const vector<double>& MatrixOptionsPricingSpace::vector_of_gammas() const
		{ // get the vector of prices; no setting 
			return d_mem_vector_of_gammas;
		}

		 // get the vector of values of parameter r; no setting
		const vector<double>& MatrixOptionsPricingSpace::r_vec() const
		{
			return d_mem_vec_r;
		}

		 // get the vector of values of parameter sig; no setting 
		const vector<double>& MatrixOptionsPricingSpace::sig_vec()const
		{
			return d_mem_vec_sig;
		}

		 // get the vector of values of parameter K; no setting 
		const vector<double>& MatrixOptionsPricingSpace::K_vec() const
		{
			return d_mem_vec_K;
		}

		 // get the vector of values of parameter T; no setting 
		const vector<double>& MatrixOptionsPricingSpace::T_vec() const
		{
			return d_mem_vec_T;
		}

		 // get the vector of values of parameter b; no setting 
		const vector<double>& MatrixOptionsPricingSpace::b_vec() const
		{
			return d_mem_vec_b;
		}

		 // get the vector of values of parameter S; no setting 
		const vector<double>& MatrixOptionsPricingSpace::S_vec() const
		{
			return d_mem_vec_S;
		}
		
		// get the option type; no setting
		const string& MatrixOptionsPricingSpace::option_type() const
		{
			return d_mem_option_type;
		}

		void MatrixOptionsPricingSpace::vector_of_prices(const vector<double>& vecPrices)
		{ // set the vector of prices
			d_mem_vector_of_prices = vecPrices;
		}

		void MatrixOptionsPricingSpace::vector_of_deltas(const vector<double>& vecDelt)
		{ // set the vector of puts
			d_mem_vector_of_deltas = vecDelt;
		}

		void MatrixOptionsPricingSpace::vector_of_gammas(const vector<double>& vecGamma)
		{ // set the vector of gammas
			d_mem_vector_of_gammas = vecGamma;
		}

		
		void MatrixOptionsPricingSpace::r_vec(const vector<double>& vecR)
		{ // set the vector of rates
			d_mem_vec_r = vecR;
		}

		
		void MatrixOptionsPricingSpace::sig_vec(const vector<double>& vecSig)
		{ // set the vector of std.dev.'s
			d_mem_vec_sig = vecSig;
		}

		
		void MatrixOptionsPricingSpace::K_vec(const vector<double>& vecK)
		{ // set the vector of strike prices
			d_mem_vec_K = vecK;
		}

		
		void MatrixOptionsPricingSpace::T_vec(const vector<double>& vecT)
		{ // set the vector of expiration dates
			d_mem_vec_T = vecT;
		}

		 
		void MatrixOptionsPricingSpace::b_vec(const vector<double>& vecb)
		{ // set the vector of cost-of-carries
			d_mem_vec_b = vecb;
		}

		 
		void MatrixOptionsPricingSpace::S_vec(const vector<double>& vec_s)
		{ // set the vector of underlying prices
			d_mem_vec_S = vec_s;
		}

		
		void MatrixOptionsPricingSpace::option_type(const string& source)
		{ // set the option type 
			d_mem_option_type = source; 
		}

	} // end of namespace Pricer
} // end of namespace MikeLawrence