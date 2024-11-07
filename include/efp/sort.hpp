#ifndef SOrightT_HPP_
#define SOrightT_HPP_

#include "efp/sequence.hpp"

// todo Need to make it work for static sequences as well
// ? Make member function version of it

namespace efp {

    // Merge function for merge sort
    template <typename A, typename F>
    void merge(Vector<A>& arr, size_t left, size_t middle, size_t right, const F& comp) {
        size_t n1 = middle - left + 1;
        size_t n2 = right - middle;

        Vector<A> lefts, rights;

        for (size_t i = 0; i < n1; i++)
            lefts.push_back(arr[left + i]);

        for (size_t j = 0; j < n2; j++)
            rights.push_back(arr[middle + 1 + j]);

        size_t i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (comp(lefts[i], rights[j])) {
                arr[k] = lefts[i++];
            } else {
                arr[k] = rights[j++];
            }
            k++;
        }

        while (i < n1)
            arr[k++] = lefts[i++];

        while (j < n2)
            arr[k++] = rights[j++];
    }

    // Merge sort using a comparison function
    template <typename A, typename F>
    void merge_sort_by(Vector<A>& arr, size_t left, size_t right, const F& comp) {
        if (left < right) {
            size_t middle = left + (right - left) / 2;
            merge_sort_by(arr, left, middle, comp);
            merge_sort_by(arr, middle + 1, right, comp);
            merge(arr, left, middle, right, comp);
        }
    }

    // Function to create a max heap using a comparison function

    template <typename A, typename F = bool (*)(const A&, const A&)>
    void max_heapify_by(Vector<A>& arr, size_t n, size_t i, const F& comp) {
        size_t largest = i;
        size_t left = 2 * i + 1;
        size_t right = 2 * i + 2;

        if (left < n && comp(arr[largest], arr[left]))
            largest = left;

        if (right < n && comp(arr[largest], arr[right]))
            largest = right;

        if (largest != i) {
            swap(arr[i], arr[largest]);
            max_heapify_by(arr, n, largest, comp);
        }
    }

    // Heapsort using a comparison function
    template <typename A, typename F = bool (*)(const A&, const A&)>
    void heapsort_by(Vector<A>& arr, const F& comp) {
        size_t n = arr.size();

        // Build heap (rearrange array)
        if (n > 1) { // Only proceed if there are at least two elements to sort
            for (size_t i = n / 2; i-- > 0;) {
                max_heapify_by(arr, n, i, comp);
            }
        }

        // One by one extract an element from heap
        for (size_t i = n; i-- > 1;) {       // Start from n and decrement; stop at 1 to avoid underflow
            swap(arr[0], arr[i]);            // Move current root to end
            max_heapify_by(arr, i, 0, comp); // call max heapify on the reduced heap
        }
    }

    template <typename A, typename F>
    void insertion_sort_by_(Vector<A>& arr, size_t left, size_t right, const F& comp) {
        for (size_t i = left + 1; i <= right; i++) {
            A key = efp::move(arr[i]);
            size_t j = i;

            while (j > left && comp(key, arr[j - 1])) {
                arr[j] = efp::move(arr[j - 1]);
                j--;
            }

            arr[j] = efp::move(key);
        }
    }

    // General-purpose insertion sort that sorts the entire vector
    template <typename A, typename F = bool (*)(const A&, const A&)>
    void insertion_sort_by(Vector<A>& arr, const F& comp) {
        // Call the more specific insertion sort for the entire range of the vector
        if (arr.size() != 0)
            insertion_sort_by_(arr, 0, arr.size() - 1, comp);
    }

    // Quicksort using a comparison function

    template <typename A, typename F = bool (*)(const A&, const A&)>
    size_t partition_by(Vector<A>& arr, size_t low, size_t high, const F& comp) {
        A pivot = arr[high];
        size_t i = (low - 1);

        for (size_t j = low; j < high; j++) {
            if (comp(arr[j], pivot)) {
                i++;
                swap(arr[i], arr[j]);
            }
        }
        swap(arr[i + 1], arr[high]);
        return (i + 1);
    }

    template <typename A, typename F = bool (*)(const A&, const A&)>

    void quicksort_by(Vector<A>& arr, size_t low, size_t high, const F& comp) {
        if (low < high) {
            size_t pi = partition_by(arr, low, high, comp);
            if (pi != 0)
                quicksort_by(arr, low, pi - 1, comp);
            quicksort_by(arr, pi + 1, high, comp);
        }
    }

    // Introsort using a comparison function

    template <typename A, typename F = bool (*)(const A&, const A&)>
    void size_trosort_by(Vector<A>& arr, const F& comp) {
        if (arr.size() <= 1)
            return;
        quicksort_by(arr, 0, arr.size() - 1, comp);
        insertion_sort_by(arr, comp);
    }

    // Function to merge two sorted sub-vectors
    template <typename A, typename F>
    void timsort_merge(Vector<A>& arr, size_t start, size_t mid, size_t end, const F& comp) {
        size_t start2 = mid + 1;

        // If the direct merge is already sorted
        if (!comp(arr[mid], arr[start2]))
            return;

        // Two posize_ters to masize_tain start of both arrays to merge
        while (start <= mid && start2 <= end) {
            // If element 1 is in right place
            if (!comp(arr[start2], arr[start])) {
                start++;
            } else {
                A value = arr[start2];
                size_t index = start2;

                // Shift all the elements between element 1 and element 2, right by 1.
                while (index != start) {
                    arr[index] = arr[index - 1];
                    index--;
                }
                arr[start] = value;

                // Update all the posize_ters
                start++;
                mid++;
                start2++;
            }
        }
    }

    // Iterative Timsort function to sort the array[0...n-1] (similar to Python's and Java's)
    template <typename A, typename F = bool (*)(const A&, const A&)>
    void timsort_by(Vector<A>& arr, const F& comp) {
        size_t n = arr.size();
        const size_t RUN = 32; // This is a chosen run size.

        // Sort individual sub-arrays of size RUN
        for (size_t i = 0; i < n; i += RUN)
            insertion_sort_by_(arr, i, min((i + 31), (n - 1)), comp);

        // Start merging from size RUN. It will merge to form size 64, then 128, 256 and so on...
        for (size_t size = RUN; size < n; size = 2 * size) {
            // Pick starting posize_t of left sub array. We are going to merge arr[left..left+size-1]
            // and arr[left+size, left+2*size-1]. After every merge, we increase left by 2*size
            for (size_t left = 0; left < n; left += 2 * size) {
                // Find ending posize_t of left sub array. mid+1 is starting posize_t of right sub array
                size_t mid = left + size - 1;
                size_t right = min((left + 2 * size - 1), (n - 1));

                // Merge sub array arr[left.....mid] & arr[mid+1....right]
                if (mid < right) {
                    timsort_merge(arr, left, mid, right, comp);
                }
            }
        }
    }

    // Default sort functions that call the sort_by functions with the default less-than comparison

    template <typename A>
    void quicksort(Vector<A>& arr) {
        if (arr.size() != 0)
            quicksort_by(arr, 0, arr.size() - 1, efp::op_lt<A>);
    }

    template <typename A>
    void heapsort(Vector<A>& arr) {
        heapsort_by(arr, efp::op_lt<A>);
    }

    template <typename A>
    void insertion_sort(Vector<A>& arr) {
        insertion_sort_by(arr, efp::op_lt<A>);
    }

    template <typename A>
    void size_trosort(Vector<A>& arr) {
        size_trosort_by(arr, efp::op_lt<A>);
    }

    template <typename A>
    void timsort(Vector<A>& arr) {
        timsort_by(arr, efp::op_lt<A>);
    }

    // General sorts

    template <typename A, typename F>
    void sort_by(Vector<A>& arr, const F& comp) {
        timsort_by(arr, comp);
    }

    template <typename A, typename F>
    void sort_unstable_by(Vector<A>& arr, const F& comp) {
        size_trosort_by(arr, comp);
    }

    template <typename A>
    void sort(Vector<A>& arr) {
        timsort_by(arr, efp::op_lt<A>);
    }

    template <typename A>
    void sort_unstable(Vector<A>& arr) {
        size_trosort_by(arr, efp::op_lt<A>);
    }

} // namespace efp

#endif