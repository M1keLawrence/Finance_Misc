// TestEuropeanOption.cpp
//
// Test program for the exact solutions of European options. 
// Check answers with Haug 2007 book on option pricing.
//
// (C) Datasim Component Technology BV 2003-20011
//
#include "MatrixOptionsPricingSpace.hpp"
#include "EuroCall.hpp"
#include "EuroPut.hpp"
#include "PerpetualAmericanCall.hpp"
#include "PerpetualAmericanPut.hpp"
#include "GlobalFunctions.hpp"
#include <vector>
#include <boost/variant.hpp>
#include <boost/variant/detail/variant_io.hpp> 
#include <map> 
#include <iostream>
#include <string>
#include <iomanip>

using namespace MikeLawrence::Pricer; 
using namespace std;
using boost::variant; 


int main()
{ 
	
	// one iterator to be used throughout the full file. 
	vector<double>::const_iterator vec_double_const_iter; // const iter for double vectors

// Group A: questions and solutions

// Question a & b)
	// Batch 1
	EuroCall indexOption_batch1(0.08, 0.30, 65.0, 0.25, 0.08);

	cout << "Batch 1 call price: " << indexOption_batch1.Price(60.0) << endl; // C = 2.13337
	cout << "Batch 1 put price: " << indexOption_batch1.PutCallParity(60.0) << endl; // P = 5.84628

	// Batch 2
	EuroCall indexOption_batch2(0.0, 0.20, 100.0, 1.0, 0.0);

	cout << "Batch 2 call price: " << indexOption_batch2.Price(100.0) << endl; // C = 7.96557
	cout << "Batch 2 put price: " << indexOption_batch2.PutCallParity(100.0) << endl; // P = 7.96557


	// Batch 3
	EuroCall indexOption_batch3(0.12, 0.50, 10.0, 1.0, 0.12); 

	cout << "Batch 3 call price: " << indexOption_batch3.Price(5.0) << endl; // C = 0.204058
	cout << "Batch 3 put price: " << indexOption_batch3.PutCallParity(5.0) << endl; // P = 4.07326

	// Batch 4
	EuroCall indexOption_batch4a(0.08, 0.30, 100.0, 30.0, 0.08);
	EuroPut indexOption_batch4b(0.08, 0.30, 100.0, 30.0, 0.08); // for testing the global put-call parity function

	cout << "Batch 4 call price: " << indexOption_batch4a.Price(100.0) << endl; // C = 92.17570
	cout << "Batch 4 put price: " << indexOption_batch4a.PutCallParity(100.0) << endl; // P = 1.24750 
	PutCallParityGlobal(indexOption_batch4a, indexOption_batch4b, 100.0); 

// Question c) see global function MeshArray, above
	vector<double> underlying_prices = MeshArray<double>(100, 5, 1); 
	cout << "\nOption price vector: \n";
	for (vec_double_const_iter = underlying_prices.begin(); vec_double_const_iter != underlying_prices.end(); ++vec_double_const_iter)
	{
		cout << indexOption_batch4a.Price(*vec_double_const_iter) << "\n"; 
	} cout << endl; // output: 1.2475, 1.23635, 1.22537, 1.21457, 1.20393

// Question d) 
	vector<double> vec_r = MeshArray<double>(0.05, 5, 0.01);
	vector<double> vec_sig = MeshArray<double>(0.30, 5, 0.02);
	vector<double> vec_K = MeshArray<double>(100.0, 5, 2.0);
	vector<double> vec_T = MeshArray<double>(30.0, 5, 0.50);
	vector<double> vec_b = MeshArray<double>(0.08, 5, 0.02);
	vector<double> vec_S = MeshArray<double>(100.0, 5, 2.0);
	MatrixOptionsPricingSpace matrix_pricer(vec_r, vec_sig, vec_K, vec_T, vec_b, vec_S, "EuroCall"); 
	try
	{
		cout << "Results for Question d, part 1, group A:" << endl;
		matrix_pricer.OptionPricingMatrix(); 

		matrix_pricer.Print("Price");
		// Results: 226.716, 330.834, 479.069, 693.658, 1008.72, 
		// MatrixOptionsPricingSpace matrix_copy(matrix_pricer); // for testing purposes
		// matrix_copy.OptionPricingMatrix();
		// matrix_copy.Print("Price"); 
	}
	catch (ArrayException& err)
	{
		cout << err.GetMessage() << endl;
	}
	catch (...)
	{
		cout << "An unhandled exception has occured." << endl; 
	}
	
	

	// Group A: section 2: question a)
	EuroCall partials_tester_C(0.1, 0.36, 100, 0.5, 0.0); // create a put and call to test the gamme and delta functions with
	EuroPut partials_tester_P(0.1, 0.36, 100, 0.5, 0.0); 
	cout << "\nDelta for a call and put, gamma for a call and put: " << endl;
	cout << partials_tester_C.Delta(105) << endl; // 0.5946
	cout << partials_tester_P.Delta(105) << endl; // -0.3566
	cout << partials_tester_C.Gamma(105) << endl; // 0.0134936
	cout << partials_tester_P.Gamma(105) << endl; // 0.0134936

	// section 2: question b) 
	vector<double> delta_mesh = MeshArray(105.0, 5, 1.0); // first value mathes the one for partials_tester_C on line 141 (as of this typing) 
	cout << "\nA monotonically increasing mesh of delta's for a given call option: " << endl; 
	for (vec_double_const_iter = delta_mesh.begin(); vec_double_const_iter != delta_mesh.end(); ++vec_double_const_iter)
	{
		cout << partials_tester_C.Delta(*vec_double_const_iter) << endl;
	} // Results: 0.594629, 0.607976, 0.621025, 0.633767, 0.646196
	cout << "\nA monotonically increasing mesh of gamma's for a given call option: " << endl;
	for (vec_double_const_iter = delta_mesh.begin(); vec_double_const_iter != delta_mesh.end(); ++vec_double_const_iter)
	{
		cout << partials_tester_C.Gamma(*vec_double_const_iter) << endl; 
	} // Results:0.0134936, 0.0131994, 0.0128966, 0.0125865, 0.0122705

	// section 2: question d)
	cout << "\nResults to section 2, question d, part a): " << endl;
	try
	{
		matrix_pricer.GreekSolverMatrix("Delta");
		matrix_pricer.Print("Delta"); // Results: 2.43196, 3.37183, 4.703, 6.61471, 9.39076, 
	}
	catch (ArrayException& err)
	{
		cout << err.GetMessage() << endl;
	}
	catch (...)
	{
		cout << "An unhandled exception occured." << endl;
	}

	cout << "\nDelta for a call and put, gamma for a call and put: " << endl;
	cout << "h = 1: " << partials_tester_C.DivDiff_Delta(105, 1) 
		<< ", h = 0.1: " << partials_tester_C.DivDiff_Delta(105, 0.1) 
		<< ", h - 0.01: " << partials_tester_C.DivDiff_Delta(105, 0.01) << endl; // h = 1: 0.59458, h = 0.1 : 0.594628, h - 0.01 : 0.594629

	cout << "h = 1: " << partials_tester_P.DivDiff_Delta(105, 1) 
		<< ", h = 0.1: " << partials_tester_P.DivDiff_Delta(105, 0.1) 
		<< ", h - 0.01: " << partials_tester_P.DivDiff_Delta(105, 0.01) << endl; // h = 1 : -0.356649, h = 0.1 : -0.356601, h - 0.01 : -0.356601



	cout << "h = 1: " << partials_tester_C.DivDiff_Gamma(105, 1) 
		<< ", h = 0.1: " << partials_tester_C.DivDiff_Gamma(105, 0.1) 
		<< ", h - 0.01: " << partials_tester_C.DivDiff_Gamma(105, 0.01) << endl; // h = 1 : 0.0134928, h = 0.1 : 0.0134936, h - 0.01 : 0.0134936


	cout << "h = 1: " << partials_tester_P.DivDiff_Gamma(105, 1) 
		<< ", h = 0.1: " << partials_tester_P.DivDiff_Gamma(105, 0.1) 
		<< ", h - 0.01: " << partials_tester_P.DivDiff_Gamma(105, 0.01) << endl; // h = 1 : 0.0134928, h = 0.1 : 0.0134936, h - 0.01 : 0.0134936
			// Thoughts: the accuracy increases to the values found by exact methods until the gains become smaller than the pre-set iostream precision 
	
// question d, part b: 
	cout << "\nResults to section 2, question d, part b): " << endl;
	cout << "\nA monotonically increasing mesh of delta's for a given call option: " << endl;
	for (vec_double_const_iter = delta_mesh.begin(); vec_double_const_iter != delta_mesh.end(); ++vec_double_const_iter)
	{
		cout << partials_tester_C.DivDiff_Delta(*vec_double_const_iter, 0.01) << endl; 
	} // Results: 0.594629, 0.607976, 0.621025, 0.633767, 0.646196
	cout << "\nA monotonically increasing mesh of gamma's for a given call option: " << endl;
	for (vec_double_const_iter = delta_mesh.begin(); vec_double_const_iter != delta_mesh.end(); ++vec_double_const_iter)
	{
		cout << partials_tester_C.DivDiff_Gamma(*vec_double_const_iter, 0.01) << endl;
	} // Results: 0.0134936, 0.0131994, 0.0128966, 0.0125865, 0.0122705
	// for fun I also tried h = 0.000000000000000000001 (well, something similar) and got the expected 
	// 'error: h is too small to compute accurately' message. 

// Group B, question a) 
	// see PerpetualAmericanPut/Call .cpp/.hpp files

	// question b)
	cout << "\nGroup B, question b: " << endl;
	PerpetualAmericanCall perpetAmerCall(0.1, 0.1, 100, 0.02); 
	PerpetualAmericanPut perpetAmerPut(0.1, 0.1, 100, 0.02);
	cout << "Perpetual American Call Price: " << perpetAmerCall.Price(110) << endl;
	cout << "Perpetual American Put Price: " << perpetAmerPut.Price(110) << endl;
	// Perpetual American Call Price: 18.5035
	// Perpetual American Put Price : 3.03106

	// question c) 
	cout << "\nGroup B, question c: mesh of perpetual american call prices: " << endl;
	vector<double> mesh_of_PerAmerCall = MeshArray(110.0, 3, 1.0);
	for (vec_double_const_iter = mesh_of_PerAmerCall.begin(); vec_double_const_iter != mesh_of_PerAmerCall.end(); ++vec_double_const_iter)
	{
		cout << perpetAmerCall.Price(*vec_double_const_iter) << ", "; 
	} cout << endl; // mesh of perpetual american call prices: 18.5035, 19.0501, 19.6078, 
	
	// question d) 
	try 
	{
		// there are two different exceptions that can be thrown here, but I have to have them in the same block or the functions can't access the object
		// Print throws 'unexpected input exception'
		// OPM throws 'size mismatch' or 'unexpected input' 
		// MM throws size mismatch
		vector<vector<double>> matrix_test = MeshMatrix<double>(matrix_pricer.r_vec(), matrix_pricer.sig_vec(), matrix_pricer.K_vec(),
			matrix_pricer.T_vec(), matrix_pricer.b_vec(), matrix_pricer.S_vec());
		MatrixOptionsPricingSpace Matrix_Pricer_test(matrix_test, "EuroCall");  
		Matrix_Pricer_test.OptionPricingMatrix();  
		Matrix_Pricer_test.Print("Price"); // results: 226.716, 330.834, 479.069, 693.658, 1008.72: they match those above, the code works!!
	}
	catch (ArrayException& err)
	{
		cout << err.GetMessage() << endl;
	}
	catch (...)
	{
		cout << "An unhandled exception has occurred" << endl;
	}
	cout << "FINAL EXAM" << endl;
	EuroPut testput(0.045, 0.51, 120, 1.45, 0); 
	cout << testput.Price(108) << endl; 
	cout << testput.Gamma(108) << endl;
	cout << testput.Delta(108) << endl;

	PerpetualAmericanPut put(0.065, 0.35, 70, .065); 
	cout << put.Price(65) << endl; 

	return 0;
}
