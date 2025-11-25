#include "GeneralFunctions.hpp"
#include <algorithm>

namespace General 
{

    // ------------------ QUICK SORT ------------------
    void quicksort(std::vector<int>& arr, int left, int right) 
    {
        if (left >= right) return;

        int i = left, j = right;
        int pivot = arr[(left + right) / 2];

        while (i <= j) 
        {
            while (arr[i] < pivot) i++;
            while (arr[j] > pivot) j--;
            if (i <= j) 
            {
                std::swap(arr[i], arr[j]);
                i++;
                j--;
            }
        }

            if (left < j) quicksort(arr, left, j);
            if (i < right) quicksort(arr, i, right);
    }

    // ------------------ MAX HEAP ------------------
    void MaxHeap::heapifyUp(int index) 
    {
        while (index > 0) 
        {
            int parent = (index - 1) / 2;
            if (data[parent] >= data[index]) break;
            std::swap(data[parent], data[index]);
            index = parent;
        }
    }

    void MaxHeap::heapifyDown(int index) 
    {
        int size = data.size();
        while (true) 
        {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int largest = index;

            if (left < size && data[left] > data[largest]) largest = left;
            if (right < size && data[right] > data[largest]) largest = right;

            if (largest == index) break;
            std::swap(data[index], data[largest]);
            index = largest;
        }
    }

    void MaxHeap::Add(int value) 
    {
        data.push_back(value);
        heapifyUp(data.size() - 1);
    }

    int MaxHeap::Remove() 
    {
        if (data.empty()) return -1;
        int maxVal = data[0];
        data[0] = data.back();
        data.pop_back();
        if (!data.empty()) heapifyDown(0);
        return maxVal;
    }

}