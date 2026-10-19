/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/qualification_batch.c
 * Purpose: Apply imported test evidence only after complete validation.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "qualification_internal.h"
#include <stdlib.h>
static int IndexOrder(const void *left, const void *right)
{
    size_t a = *(const size_t *)left, b = *(const size_t *)right;
    return a < b ? -1 : a > b ? 1 : 0;
}
UmiStatus UmiReleaseQualificationRecordTests(UmiReleaseQualification *run,
    const UmiReleaseQualificationIdentity *identity,
    const UmiReleaseQualificationNamedResult *results, size_t count,
    const UmiReleaseQualificationResult *testProcess)
{
    UmiStatus status = UmiQualificationMatch(run, identity);
    if (status != UMI_STATUS_OK) return status;
    if (count != 0U && results == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (run->closed || run->executionStatus != UMI_STATUS_OK || !run->buildRecorded) return UMI_STATUS_INVALID_STATE;
    if (count > run->selection.tests) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (testProcess != NULL) {
        status = UmiQualificationResultValid(testProcess);
        if (status != UMI_STATUS_OK) return status;
        if (count != run->selection.tests || count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
        if (testProcess->outcome == UMI_QUALIFICATION_PASSED && !testProcess->exitCodeKnown)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (run->testProcessRecorded) return UMI_STATUS_ALREADY_EXISTS;
    }
    if (count == 0U) return UMI_STATUS_OK;
    size_t *indices = malloc(count * sizeof(*indices));
    if (indices == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Validate the entire batch before any record is written. This protects
     * existing evidence when the last imported name is unknown or duplicated. */
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i) {
        if (results[i].name == NULL) { status = UMI_STATUS_INVALID_ARGUMENT; break; }
        status = UmiQualificationResultValid(&results[i].result);
        if (status != UMI_STATUS_OK) break;
        if (results[i].result.outcome == UMI_QUALIFICATION_PASSED &&
            !results[i].result.exitCodeKnown && testProcess == NULL) {
            status = UMI_STATUS_INVALID_ARGUMENT; break;
        }
        indices[i] = UmiQualificationFindTest(run, results[i].name);
        if (indices[i] == SIZE_MAX) status = UMI_STATUS_NOT_FOUND;
        else if (run->tests[indices[i]].recorded) status = UMI_STATUS_ALREADY_EXISTS;
    }
    if (status == UMI_STATUS_OK) {
        qsort(indices, count, sizeof(*indices), IndexOrder);
        for (size_t i = 1U; i < count; ++i)
            if (indices[i] == indices[i - 1U]) { status = UMI_STATUS_ALREADY_EXISTS; break; }
    }
    if (status == UMI_STATUS_OK) {
        if (testProcess != NULL) {
            run->testProcess = *testProcess;
            run->testProcessRecorded = true;
        }
        /* The run has one mutation owner, so validated names cannot change
         * between preflight and this allocation-free commit. */
        for (size_t i = 0U; i < count; ++i) {
            size_t index = UmiQualificationFindTest(run, results[i].name);
            run->tests[index].result = results[i].result;
            run->tests[index].recorded = true;
        }
    }
    free(indices);
    return status;
}
