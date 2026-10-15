/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/policy_batch/test_batch.c
 * PURPOSE: Check complete-group publication, allocation refusal and temporary-storage release.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../value_archive/construction_cases.h"
#include "../../src/base/policy_batch_internal.h"

typedef struct PolicyObservation { int value; } PolicyObservation;
typedef struct PolicyResult { int value; } PolicyResult;
static unsigned allocation_attempts, outstanding_allocations, evaluated_rows;
static bool refuse_allocation;

/* The allocator seam exists only in this translation unit. Production callers
 * continue to use the Framework's ordinary temporary-allocation contract. */
static void *PolicyAllocate(size_t size)
{
    ++allocation_attempts;
    if (refuse_allocation) return NULL;
    void *memory = malloc(size);
    if (memory != NULL) ++outstanding_allocations;
    return memory;
}
static void PolicyRelease(void *memory)
{
    if (memory != NULL) --outstanding_allocations;
    free(memory);
}
static UmiStatus PolicyEvaluate(const PolicyObservation *input, PolicyResult *output)
{
    ++evaluated_rows;
    output->value = input->value;
    /* Deliberately fail after touching the candidate. A later failed row must
     * still leave every caller-owned result exactly as it was. */
    return input->value < 0 ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
}
#define malloc PolicyAllocate
#define free PolicyRelease
UMI_DEFINE_POLICY_BATCH(PolicyEvaluateGroup, PolicyEvaluate, PolicyObservation, PolicyResult)
#undef free
#undef malloc

int main(void)
{
    PolicyObservation inputs[2] = {{10}, {20}};
    PolicyResult outputs[3] = {{1}, {2}, {3}};
    unsigned char before[sizeof(outputs)];
    int result = 1;
    memcpy(before, outputs, sizeof(outputs));

    /* Empty groups and invalid capacities must not allocate or evaluate rows. */
    UMI_CONSTRUCTION_REQUIRE(PolicyEvaluateGroup(NULL, 0U, NULL, 0U) == UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(PolicyEvaluateGroup(NULL, 1U, outputs, 3U) == UMI_STATUS_INVALID_ARGUMENT);
    UMI_CONSTRUCTION_REQUIRE(PolicyEvaluateGroup(inputs, 1U, NULL, 3U) == UMI_STATUS_INVALID_ARGUMENT);
    UMI_CONSTRUCTION_REQUIRE(PolicyEvaluateGroup(inputs, 2U, outputs, 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
    UMI_CONSTRUCTION_REQUIRE(PolicyEvaluateGroup(inputs, 4097U, outputs, 4097U) == UMI_STATUS_CAPACITY_EXCEEDED);
    UMI_CONSTRUCTION_REQUIRE(allocation_attempts == 0U && evaluated_rows == 0U);

    refuse_allocation = true;
    UMI_CONSTRUCTION_REQUIRE(PolicyEvaluateGroup(inputs, 2U, outputs, 3U) == UMI_STATUS_OUT_OF_MEMORY);
    UMI_CONSTRUCTION_REQUIRE(allocation_attempts == 1U && outstanding_allocations == 0U && evaluated_rows == 0U);
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, outputs, sizeof(outputs)) == 0);

    refuse_allocation = false;
    inputs[1].value = -1;
    UMI_CONSTRUCTION_REQUIRE(PolicyEvaluateGroup(inputs, 2U, outputs, 3U) == UMI_STATUS_INVALID_ARGUMENT);
    UMI_CONSTRUCTION_REQUIRE(evaluated_rows == 2U && outstanding_allocations == 0U);
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, outputs, sizeof(outputs)) == 0);

    inputs[1].value = 20;
    UMI_CONSTRUCTION_REQUIRE(PolicyEvaluateGroup(inputs, 2U, outputs, 3U) == UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(outputs[0].value == 10 && outputs[1].value == 20 && outputs[2].value == 3);
    UMI_CONSTRUCTION_REQUIRE(outstanding_allocations == 0U);
    result = 0;
construction_cleanup:
    return result;
}
