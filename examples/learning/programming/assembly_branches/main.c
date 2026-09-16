/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/assembly_branches/main.c
 *
 * PURPOSE:
 *   Assembly registers, comparisons and branches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../assembly_contract.h"

static uint64_t MaximumC(uint64_t left, uint64_t right)
{
    return left>=right?left:right;
}
int main(void)
{
    const uint64_t pairs[][2]={{0U,0U},{4U,9U},{9U,4U},{UINT64_MAX,1U},{1U,UINT64_MAX}};
    for (size_t index=0U; index<sizeof(pairs)/sizeof(pairs[0]); ++index) {
        uint64_t expected=MaximumC(pairs[index][0],pairs[index][1]);
#ifdef UMICOM_LAB_NATIVE_ASSEMBLY
        if (LabMaximumAsm(pairs[index][0],pairs[index][1])!=expected) return EXIT_FAILURE;
#else
        if (expected!=pairs[index][0] && expected!=pairs[index][1]) return EXIT_FAILURE;
#endif
    }
#ifdef UMICOM_LAB_NATIVE_ASSEMBLY
    puts("native Assembly matched C for five unsigned comparisons");
#else
    puts("C-only mode: enable UMICOM_LEARNING_BUILD_ASSEMBLY for native comparison");
#endif
    puts("PASS: assembly_branches");
    return EXIT_SUCCESS;
}
