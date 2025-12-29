/**
 * test program for our ProductServices
 */

#include <iostream>
#include "products.hpp"
#include "productserviceP3.hpp"

using namespace std;


// Helper function to print a vector of Bonds
void PrintBonds(vector<Bond>& bonds, string comment)
{
    cout << "--- " << comment << " (" << bonds.size() << " found) ---" << endl;
    for (size_t i = 0; i < bonds.size(); ++i)
    {
        cout << bonds[i] << endl;
    }
}

// Helper function to print a vector of Swaps
void PrintSwaps(vector<IRSwap>& swaps, string comment)
{
    cout << "--- " << comment << " (" << swaps.size() << " found) ---" << endl;
    for (size_t i = 0; i < swaps.size(); ++i)
    {
        cout << swaps[i] << endl;
    }
}

int main()
{
	std::cout << "Testing Product Services" << std::endl;

	// Create the 10Y treasury note
	date maturityDate(2025, Nov, 16);
	string cusip = "912828M56";
	Bond treasuryBond(cusip, CUSIP, "T", 2.25, maturityDate);

	// Create the 2Y treasury note
	date maturityDate2(2017, Nov, 5);
	string cusip2 = "912828TW0";
	Bond treasuryBond2(cusip2, CUSIP, "T", 0.75, maturityDate2);

	// Create a BondProductService
	BondProductService *bondProductService = new BondProductService();

	// Add the 10Y note to the BondProductService and retrieve it from the service
	bondProductService->Add(treasuryBond);
	Bond bond = bondProductService->GetData(cusip);
	cout << "CUSIP: " << bond.GetProductId() << " ==> " << bond << endl;

	// Add the 2Y note to the BondProductService and retrieve it from the service
	bondProductService->Add(treasuryBond2);
	bond = bondProductService->GetData(cusip2);
	cout << "CUSIP: " << bond.GetProductId() << " ==> " << bond << endl;

	// Create the Spot 10Y Outright Swap
	date effectiveDate(2015, Nov, 16);
	date terminationDate(2025, Nov, 16);
	string outright10Y = "Spot-Outright-10Y";
	IRSwap outright10YSwap(outright10Y, THIRTY_THREE_SIXTY, THIRTY_THREE_SIXTY, SEMI_ANNUAL, LIBOR, TENOR_3M, effectiveDate, terminationDate, USD, 10, SPOT, OUTRIGHT);

	// Create the IMM 2Y Outright Swap
	date effectiveDate2(2015, Dec, 20);
	date terminationDate2(2017, Dec, 20);
	string imm2Y = "IMM-Outright-2Y";
	IRSwap imm2YSwap(imm2Y, THIRTY_THREE_SIXTY, THIRTY_THREE_SIXTY, SEMI_ANNUAL, LIBOR, TENOR_3M, effectiveDate2, terminationDate2, USD, 2, IMM, OUTRIGHT);

	// Create a IRSwapProductService
	IRSwapProductService *swapProductService = new IRSwapProductService();

	// Add the Spot 10Y Outright Swap to the IRSwapProductService and retrieve it from the service
	swapProductService->Add(outright10YSwap);
	IRSwap swap = swapProductService->GetData(outright10Y);
	cout << "Swap: " << swap.GetProductId() << " == > " << swap << endl;

	// Add the IMM 2Y Outright Swap to the IRSwapProductService and retrieve it from the service
	swapProductService->Add(imm2YSwap);
	swap = swapProductService->GetData(imm2Y);
	cout << "Swap: " << swap.GetProductId() << " == > " << swap << endl;




	
	// --- TEST BOND SEARCH ---
	cout << "\n--- BEGIN BOND SEARCH TESTS ---\n" << endl;

    // Test: GetBonds(string& _ticker)
    // Note: Must use a variable, not a literal, because the signature is string&
    string searchTicker = "T"; 
    vector<Bond> resultBonds = bondProductService->GetBonds(searchTicker);
    PrintBonds(resultBonds, "Bonds with Ticker 'T'");

    // --- TEST SWAP SEARCH ---
	cout << "\n--- BEGIN SWAP SEARCH TESTS ---\n" << endl;

    // Test: GetSwapsGreaterThan(int _termYears)
    // Should return only the 10Y swap
    vector<IRSwap> longTermSwaps = swapProductService->GetSwapsGreaterThan(5);
    PrintSwaps(longTermSwaps, "Swaps > 5 Years");

    // Test: GetSwapsLessThan(int _termYears)
    // Should return only the 2Y swap
    vector<IRSwap> shortTermSwaps = swapProductService->GetSwapsLessThan(5);
    PrintSwaps(shortTermSwaps, "Swaps < 5 Years");

    // Test: GetSwaps(SwapType _swapType)
    // Should return only the IMM swap
    vector<IRSwap> immSwaps = swapProductService->GetSwaps(IMM);
    PrintSwaps(immSwaps, "IMM Swaps");

    // Test: GetSwaps(SwapType _swapType)
    // Should return only the Spot swap
    vector<IRSwap> spotSwaps = swapProductService->GetSwaps(SPOT);
    PrintSwaps(spotSwaps, "Spot Swaps");

    // Test: GetSwaps(SwapLegType _swapLegType)
    // Should return both swaps (both are OUTRIGHT)
    vector<IRSwap> outrightSwaps = swapProductService->GetSwaps(OUTRIGHT);
    PrintSwaps(outrightSwaps, "Outright Swaps");

    // Test: GetSwaps(DayCountConvention)
    // Should return both (both are 30/360)
    vector<IRSwap> dayCountSwaps = swapProductService->GetSwaps(THIRTY_THREE_SIXTY);
    PrintSwaps(dayCountSwaps, "Swaps with 30/360 Fixed Leg");

	return 0;
}
