#include <iostream>

void partition(int* v, int& i, int& j, int low, int high)
{
    i = low;
    j = high;
    int pivot = v[(low + high) / 2];
    do {
        while (v[i] < pivot)
            i++;
        while (v[j] > pivot)
            j--;
        if (i <= j) {
            std::swap(v[i], v[j]);
            i++;
            j--;
        }
    } while (i <= j);
}

void quicksort(int* v, int low, int high)
{
    int i, j;
    partition(v, i, j, low, high);
    if (low < j)
        quicksort(v, low, j);
    if (i < high)
        quicksort(v, i, high);
}

int main(void)
{
    int v[] = { 3, 2, 1, 0, 2, 5, 6, 7, 1, 2, 0 };

    quicksort(v, 0, sizeof(v) / sizeof(v[0]) - 1);

    for (const auto& i : v)
        std::cout << i << ' ';
    std::cout << '\n';

    return 0;
}
