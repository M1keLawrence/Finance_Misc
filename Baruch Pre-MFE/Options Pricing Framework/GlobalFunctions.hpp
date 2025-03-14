#ifndef GlobalFunctions_hpp
#define GlobalFunctions_hpp
#include <vector>
#include "EuroCall.hpp"
#include "EuroPut.hpp"
#include "SizeMismatchException.hpp" 
#include <boost\variant.hpp> // Variant class
#include "boost/tuple/tuple.hpp"
#include <iostream>
using  boost::variant; // for readability

namespace MikeLawrence
{
	namespace Pricer
	{
		template<typename T>
		vector<T> MeshArray(const T start, const int num_increments, const T mesh_size)
		{

			vector<T> mesh;

			for (int i = 0; i != num_increments + 1; i++)
			{
				for (int j = 0; j != num_increments; ++j)
				{
					mesh.push_back(start + (mesh_size * j));
				}

				return mesh;
			}
		}

		template<typename T> 
		vector<vector<T>> MeshMatrix(const vector<T> input_r, const vector<T> input_sig, const vector<T> input_K, const vector<T> input_T, 
									 const vector<T> input_b, const vector<T> input_S)
		{
			// *** IMPORTANT *** use random access iterators and just set them to the beginning of the vector of vectors
			// then just use the ability of that iterator to return a reference to any element of the associated collection. 
			vector<vector<double>> matrix_of_outputs(input_r.size()); // create a matrix with the same column space as the inputs  

			if (input_r.size() != input_sig.size() || input_r.size() != input_K.size() || input_r.size() != input_T.size() || input_r.size() != input_b.size() || input_r.size() != input_S.size())
			{
				throw SizeMismatchException(); // throw a size mismatch exception object
			}
			else
			{
				// Now, take the inputs and translate them to a matrix ready for input for a pricing function. 
				// Each new vector contains a single value of each parameter necessary for pricing. 

				for (int i = 0; i < input_r.size(); i++)
				{// This feels wrong, but I'm leaving it as an int and not an iterator because I don't know 
				 // how to iterate through six vectors at once using iterators. 

					matrix_of_outputs[i].push_back(input_r[i]);
					matrix_of_outputs[i].push_back(input_sig[i]);
					matrix_of_outputs[i].push_back(input_K[i]);
					matrix_of_outputs[i].push_back(input_T[i]);
					matrix_of_outputs[i].push_back(input_b[i]);
					matrix_of_outputs[i].push_back(input_S[i]);
					//cout << matrix_of_outputs[i][i] << endl;
					//cout << matrix_of_outputs[i][5] << endl; 

				}

				return matrix_of_outputs;
			}
		}

		void print_vector(const vector<double>& vec_input)
		{
			vector<double>::const_iterator vec_double_const_iter; // const iter for double vectors
			int output_cnt = 0;
			for (vec_double_const_iter = vec_input.begin(); vec_double_const_iter != vec_input.end(); ++vec_double_const_iter) // loop through and print out each element of the vector. 
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

		void PutCallParityGlobal(EuroCall& c, EuroPut& p, double U)
		{ // a second put-call parity function to test if parity holds for a given put/call combination.

			if (c.R() == p.R() && c.Sig() == p.Sig() && c.K() == p.K() && c.T() == p.T() && c.B() == p.B())
			{

				// Input to if() #1: (c.Price(U) + (c.Get_K() * exp((-c.Get_r()) * c.Get_T()))) == (p.Price(U) + U)
				if ((round(1000 * c.PutCallParity(U)) == round(1000 * p.Price(U))) && (round(1000 * p.PutCallParity(U)) == round(1000 * c.Price(U))))
				{
					cout << "Put-Call Parity holds.\n"
						<< "Put price: " << p.Price(U) << ".\n"
						<< "Call price: " << c.Price(U) << endl;
				}
				else {
					cout << "Put-Call Parity is violated." << endl; 
					//cout << "Test: " << (c.Price(U) + (c.Get_K() * exp((-c.Get_r()) * c.Get_T())))
					//	<< " does not equal: " << (p.Price(U) + U) << endl;
				}

			}
			else {
				cout << "The options provided are not equivalent." << endl;
			}
		}
	}
}
#endif