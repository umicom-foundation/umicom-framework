/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/policy_batch/cases.h
 * PURPOSE: Verify complete policy refreshes, bounded input and refusal preservation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_POLICY_BATCH_CASES_H
#define UMICOM_TEST_POLICY_BATCH_CASES_H
#include "../value_archive/construction_cases.h"
#include <math.h>

/* Bind each existing domain fixture to its real public policy. A second,
 * deliberately invalid observation proves that successful earlier rows cannot
 * escape when the complete refresh fails. No provider or order is invoked. */
#define UMI_TEST_POLICY_BATCH(Name, Scalar, Batch, Input, Output, NumberField) \
static int Name(const Input *sample) \
{ \
    Input inputs[2] = {*sample, *sample}; \
    Output output[3], scalar; \
    unsigned char before[sizeof(output)]; \
    int result = 1; \
    memset(output, 0xa5, sizeof(output)); \
    memcpy(before, output, sizeof(output)); \
    UMI_CONSTRUCTION_REQUIRE(Batch(NULL, 0U, NULL, 0U) == UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(Batch(NULL, 1U, output, 3U) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_CONSTRUCTION_REQUIRE(Batch(inputs, 1U, NULL, 3U) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_CONSTRUCTION_REQUIRE(Batch(inputs, 2U, output, 1U) == UMI_STATUS_CAPACITY_EXCEEDED); \
    UMI_CONSTRUCTION_REQUIRE(Batch(inputs, 4097U, output, 4097U) == UMI_STATUS_CAPACITY_EXCEEDED); \
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, output, sizeof(output)) == 0); \
    inputs[1].NumberField = NAN; \
    UMI_CONSTRUCTION_REQUIRE(Batch(inputs, 2U, output, 3U) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, output, sizeof(output)) == 0); \
    inputs[1] = *sample; \
    inputs[1].NumberField = INFINITY; \
    UMI_CONSTRUCTION_REQUIRE(Batch(inputs, 2U, output, 3U) == UMI_STATUS_INVALID_ARGUMENT); \
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, output, sizeof(output)) == 0); \
    char oversized[sizeof(output[0].label) + 1U]; \
    memset(oversized, 'x', sizeof(oversized)); oversized[sizeof(oversized) - 1U] = '\0'; \
    inputs[1] = *sample; inputs[1].label = oversized; \
    UMI_CONSTRUCTION_REQUIRE(Batch(inputs, 2U, output, 3U) == UMI_STATUS_CAPACITY_EXCEEDED); \
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, output, sizeof(output)) == 0); \
    inputs[1] = *sample; \
    UMI_CONSTRUCTION_REQUIRE(Batch(inputs, 2U, output, 3U) == UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(Scalar(sample, &scalar) == UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(strcmp(output[0].id, scalar.id) == 0 && \
        strcmp(output[1].label, scalar.label) == 0 && output[0].revision == sample->revision); \
    UMI_CONSTRUCTION_REQUIRE(memcmp(&output[2], before + 2U * sizeof(Output), sizeof(Output)) == 0); \
    /* Borrowing a label from the destination must survive scalar staging. */ \
    inputs[0].label = output[0].label; \
    UMI_CONSTRUCTION_REQUIRE(Scalar(&inputs[0], &output[0]) == UMI_STATUS_OK); \
    UMI_CONSTRUCTION_REQUIRE(strcmp(output[0].label, scalar.label) == 0); \
    result = 0; \
construction_cleanup: \
    return result; \
}
#endif
