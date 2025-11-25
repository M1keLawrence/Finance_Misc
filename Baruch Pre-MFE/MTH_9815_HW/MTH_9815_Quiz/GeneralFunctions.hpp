#pragma once
#include <vector>

namespace General {
    // Exercise 1: quicksort
    void quicksort(std::vector<int>& arr, int left, int right);

    // Exercise 2 and 3: Max-Heap class
    class MaxHeap {
        private:
            std::vector<int> data;
            void heapifyUp(int index);
            void heapifyDown(int index);

        public:
            void Add(int value); // insert
            int Remove(); // bonus: remove max
            bool Empty() const { return data.empty(); }
    };
}