/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/search_sort/main.c
 *
 * PURPOSE:
 *   Search, sorting and algorithm cost.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void InsertionSort(int *values, size_t count)
{
    for (size_t index=1U; index<count; ++index) {
        int item=values[index]; size_t position=index;
        while (position>0U && values[position-1U]>item) {
            values[position]=values[position-1U]; --position;
        }
        values[position]=item;
    }
}
static size_t FindSorted(const int *values, size_t count, int wanted)
{
    size_t low=0U, high=count; /* Half-open interval [low, high). */
    while (low<high) {
        size_t middle=low+(high-low)/2U;
        if (values[middle]==wanted) return middle;
        if (values[middle]<wanted) low=middle+1U; else high=middle;
    }
    return SIZE_MAX;
}
int main(void)
{
    int values[]={9,1,5,1};
    InsertionSort(values,4U);
    if (values[0]!=1 || values[1]!=1 || values[2]!=5 || values[3]!=9) return EXIT_FAILURE;
    size_t found=FindSorted(values,4U,5);
    if (found==SIZE_MAX || values[found]!=5 || FindSorted(values,4U,8)!=SIZE_MAX ||
        FindSorted(values,0U,5)!=SIZE_MAX) return EXIT_FAILURE;
    puts("sorted 1 1 5 9; found 5; missing 8 rejected");
    puts("PASS: search_sort");
    return EXIT_SUCCESS;
}
