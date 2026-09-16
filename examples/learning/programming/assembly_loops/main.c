/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/assembly_loops/main.c
 *
 * PURPOSE:
 *   Assembly loops, memory and empty input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../assembly_contract.h"

static uint64_t SumC(const uint64_t *values, size_t count)
{
    uint64_t total=0U;
    for (size_t index=0U; index<count; ++index) total+=values[index];
    return total; /* Deliberate modulo-2^64 sum; not a checked cash total. */
}
int main(void)
{
    const uint64_t values[]={10U,20U,30U,40U};
    const uint64_t wrap[]={UINT64_MAX,1U};
    if (SumC(values,4U)!=100U || SumC(NULL,0U)!=0U || SumC(wrap,2U)!=0U)
        return EXIT_FAILURE;
#ifdef UMICOM_LAB_NATIVE_ASSEMBLY
    if (LabSumAsm(values,4U)!=100U || LabSumAsm(NULL,0U)!=0U ||
        LabSumAsm(values,1U)!=10U || LabSumAsm(wrap,2U)!=0U) return EXIT_FAILURE;
    puts("native Assembly: empty, one-item, four-item and modulo sum checked");
#else
    puts("C-only mode: enable UMICOM_LEARNING_BUILD_ASSEMBLY for native comparison");
#endif
    puts("PASS: assembly_loops");
    return EXIT_SUCCESS;
}
