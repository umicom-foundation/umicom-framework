/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/qualification.c
 * Purpose: Own an immutable selection while collecting one attempt's exact outcomes.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "qualification_internal.h"
#include <stdlib.h>
#include <string.h>

void UmiReleaseQualificationDestroy(UmiReleaseQualification *run)
{
    if (run == NULL) return;
    for (size_t i = 0U; i < run->count; ++i) free((void *)run->items[i].name);
    free(run->items);
    free(run->tests);
    free(run);
}
UmiStatus UmiReleaseQualificationCreate(const UmiReleaseImpactPlan *plan,
    const UmiReleaseSourceObservation *before, const char *execution,
    const char *toolchain, UmiReleaseQualification **outRun)
{
    if (plan == NULL || before == NULL || outRun == NULL || *outRun != NULL ||
        !UmiQualificationDigestValid(execution) || !UmiQualificationDigestValid(toolchain))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(UmiReleaseImpactInventoryFingerprint(plan),
        UmiReleaseSourceObservationInventoryFingerprint(before)) != 0) return UMI_STATUS_INVALID_STATE;
    UmiReleaseImpactSummary selection = {0};
    UmiStatus status = UmiReleaseImpactDescribe(plan, &selection);
    if (status != UMI_STATUS_OK) return status;
    if (selection.noWork) return UMI_STATUS_UNAVAILABLE;
    UmiReleaseQualification *run = calloc(1U, sizeof(*run));
    if (run == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    size_t count = UmiReleaseImpactCount(plan);
    run->selection = selection;
    run->testStart = selection.targets;
    run->items = calloc(count == 0U ? 1U : count, sizeof(*run->items));
    run->tests = calloc(selection.tests == 0U ? 1U : selection.tests, sizeof(*run->tests));
    if (run->items == NULL || run->tests == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    size_t bytes = 0U;
    /* The public plan sorts targets before tests. Copy full names rather than
     * fitting them into UI-sized labels: exact test identity is a coverage boundary. */
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i) {
        const UmiReleaseImpactItem *item = UmiReleaseImpactAt(plan, i);
        size_t length = strlen(item->name) + 1U;
        if (length > UMI_RELEASE_INVENTORY_TEXT_LIMIT - bytes) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        char *name = malloc(length);
        if (name == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; break; }
        memcpy(name, item->name, length);
        run->items[run->count++] = (UmiReleaseImpactItem){item->kind, name, item->reasons};
        bytes += length;
    }
    if (status == UMI_STATUS_OK) {
        memcpy(run->identity.execution, execution, 65U);
        memcpy(run->toolchain, toolchain, 65U);
        memcpy(run->inventory, UmiReleaseImpactInventoryFingerprint(plan), 65U);
        memcpy(run->source, UmiReleaseSourceObservationFingerprint(before), 65U);
        status = UmiQualificationPlanFingerprint(run);
    }
    if (status == UMI_STATUS_OK) *outRun = run;
    else UmiReleaseQualificationDestroy(run);
    return status;
}
UmiStatus UmiReleaseQualificationGetIdentity(const UmiReleaseQualification *run,
    UmiReleaseQualificationIdentity *outIdentity)
{
    if (run == NULL || outIdentity == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outIdentity = run->identity;
    return UMI_STATUS_OK;
}
size_t UmiReleaseQualificationCount(const UmiReleaseQualification *run)
{
    return run == NULL ? 0U : run->count;
}
const UmiReleaseImpactItem *UmiReleaseQualificationAt(const UmiReleaseQualification *run, size_t index)
{
    return run == NULL || index >= run->count ? NULL : &run->items[index];
}
size_t UmiQualificationFindTest(const UmiReleaseQualification *run, const char *name)
{
    size_t low = 0U, high = run->selection.tests;
    while (low < high) {
        size_t middle = low + (high - low) / 2U;
        int comparison = strcmp(run->items[run->testStart + middle].name, name);
        if (comparison == 0) return middle;
        if (comparison < 0) low = middle + 1U;
        else high = middle;
    }
    return SIZE_MAX;
}
UmiStatus UmiQualificationResultValid(const UmiReleaseQualificationResult *result)
{
    if (result == NULL || (unsigned)result->outcome > (unsigned)UMI_QUALIFICATION_DISABLED ||
        (unsigned)result->status > (unsigned)UMI_STATUS_BUSY ||
        !UmiQualificationDigestValid(result->logSha256)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!result->exitCodeKnown && result->exitCode != 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (result->outcome == UMI_QUALIFICATION_PASSED &&
        (result->status != UMI_STATUS_OK || result->exitCode != 0)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus UmiReleaseQualificationRecordBuild(UmiReleaseQualification *run,
    const UmiReleaseQualificationIdentity *identity, const UmiReleaseQualificationResult *result)
{
    UmiStatus status = UmiQualificationMatch(run, identity);
    if (status != UMI_STATUS_OK) return status;
    if (run->closed || run->executionStatus != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
    status = UmiQualificationResultValid(result);
    if (status != UMI_STATUS_OK) return status;
    if (result->outcome == UMI_QUALIFICATION_PASSED && !result->exitCodeKnown) return UMI_STATUS_INVALID_ARGUMENT;
    if (run->buildRecorded) return UMI_STATUS_ALREADY_EXISTS;
    run->build = *result;
    run->buildRecorded = true;
    return UMI_STATUS_OK;
}
UmiStatus UmiReleaseQualificationRecordTest(UmiReleaseQualification *run,
    const UmiReleaseQualificationIdentity *identity, const char *testName,
    const UmiReleaseQualificationResult *result)
{
    UmiStatus status = UmiQualificationMatch(run, identity);
    if (status != UMI_STATUS_OK) return status;
    if (testName == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (run->closed || run->executionStatus != UMI_STATUS_OK || !run->buildRecorded) return UMI_STATUS_INVALID_STATE;
    status = UmiQualificationResultValid(result);
    if (status != UMI_STATUS_OK) return status;
    if (result->outcome == UMI_QUALIFICATION_PASSED && !result->exitCodeKnown)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = UmiQualificationFindTest(run, testName);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (run->tests[index].recorded) return UMI_STATUS_ALREADY_EXISTS;
    run->tests[index].result = *result;
    run->tests[index].recorded = true;
    return UMI_STATUS_OK;
}
UmiStatus UmiReleaseQualificationGetBuild(const UmiReleaseQualification *run,
    UmiReleaseQualificationResult *outResult)
{
    if (run == NULL || outResult == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!run->buildRecorded) return UMI_STATUS_NOT_FOUND;
    *outResult = run->build;
    return UMI_STATUS_OK;
}
UmiStatus UmiReleaseQualificationGetTest(const UmiReleaseQualification *run,
    const char *testName, UmiReleaseQualificationResult *outResult)
{
    if (run == NULL || testName == NULL || outResult == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = UmiQualificationFindTest(run, testName);
    if (index == SIZE_MAX || !run->tests[index].recorded) return UMI_STATUS_NOT_FOUND;
    *outResult = run->tests[index].result;
    return UMI_STATUS_OK;
}

const char *UmiReleaseQualificationSourceFingerprint(const UmiReleaseQualification *run)
{
    return run == NULL ? NULL : run->source;
}

UmiStatus UmiReleaseQualificationGetTestProcess(const UmiReleaseQualification *run,
    UmiReleaseQualificationResult *outResult)
{
    if (run == NULL || outResult == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!run->testProcessRecorded) return UMI_STATUS_NOT_FOUND;
    *outResult = run->testProcess;
    return UMI_STATUS_OK;
}
