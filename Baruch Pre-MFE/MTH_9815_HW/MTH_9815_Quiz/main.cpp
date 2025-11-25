#include <iostream>
#include <vector>
#include "GeneralFunctions.hpp"

int main() {
    using namespace General;

    // ------- User Input for Quicksort -------
    int n;
    std::cout << "Enter number of elements to sort: ";
    std::cin >> n;

    std::vector<int> arr(n);
    std::cout << "Enter " << n << " integers: ";
    for (int i = 0; i < n; i++) {
    std::cin >> arr[i];
    }

    std::cout << "Original array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << "\n\n";

    quicksort(arr, 0, arr.size() - 1);

    std::cout << "Sorted array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << "\n\n";

    // ------- Test MaxHeap -------
    MaxHeap heap;

    std::cout << "Adding to heap: 10 4 15 7";
    heap.Add(10);
    heap.Add(4);
    heap.Add(15);
    heap.Add(7);

    std::cout << "Removing elements (in max order): ";
    while (!heap.Empty()) 
    {
    std::cout << heap.Remove() << " ";
    }
    std::cout << "\n";

    return 0;
}