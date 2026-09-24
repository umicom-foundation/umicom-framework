/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/developer_operations_internal.c
 *
 * PURPOSE:
 *   Evaluate reusable developer-operation view policies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "developer_operations_internal.h"

#include <stdio.h>
#include <string.h>

static double UmiDeveloperOperationsPercent(uint64_t value, uint64_t total)
{
    if (total == 0U) return 0.0;
    if (value >= total) return 100.0;
    return ((double)value / (double)total) * 100.0;
}

UmiStatus UmiDeveloperOperationsEvaluatePolicy(
    const UmiDeveloperOperationPolicy *policy,
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot)
{
    double threshold;

    if (policy == NULL || policy->id == NULL || policy->label == NULL ||
        input == NULL || outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    UmiDeveloperOperationSnapshotInit(outSnapshot);
    (void)snprintf(outSnapshot->id, sizeof(outSnapshot->id), "%s", policy->id);
    (void)snprintf(
        outSnapshot->label,
        sizeof(outSnapshot->label),
        "%s",
        input->label != NULL && input->label[0] != '\0'
            ? input->label
            : policy->label);

    outSnapshot->primary = input->primary;
    outSnapshot->secondary = input->secondary;
    outSnapshot->total = input->total;
    outSnapshot->missed =
        input->total > input->primary ? input->total - input->primary : 0U;
    outSnapshot->percent =
        UmiDeveloperOperationsPercent(input->primary, input->total);
    outSnapshot->delta = outSnapshot->percent - input->baseline;
    outSnapshot->active = input->active != 0;
    outSnapshot->blocked = input->blocked != 0;
    outSnapshot->revision = input->revision;
    threshold = input->threshold > 0.0
        ? input->threshold
        : policy->defaultThreshold;

    switch (policy->kind) {
    case UMI_DEVELOPER_OPERATION_RATIO:
        outSnapshot->ready = input->total > 0U && !outSnapshot->blocked;
        outSnapshot->attention = outSnapshot->ready &&
            threshold > 0.0 && outSnapshot->percent < threshold;
        break;
    case UMI_DEVELOPER_OPERATION_DELTA:
        outSnapshot->ready = input->total > 0U && !outSnapshot->blocked;
        outSnapshot->attention = outSnapshot->ready && outSnapshot->delta < 0.0;
        break;
    case UMI_DEVELOPER_OPERATION_THRESHOLD:
        outSnapshot->ready = input->total > 0U &&
            outSnapshot->percent >= threshold && !outSnapshot->blocked;
        outSnapshot->attention = !outSnapshot->ready;
        break;
    case UMI_DEVELOPER_OPERATION_ACTIVITY:
        outSnapshot->ready = outSnapshot->active && !outSnapshot->blocked;
        outSnapshot->attention = outSnapshot->blocked;
        break;
    case UMI_DEVELOPER_OPERATION_COUNT:
        outSnapshot->ready = !outSnapshot->blocked;
        outSnapshot->attention = input->secondary > 0U;
        break;
    case UMI_DEVELOPER_OPERATION_CAPACITY:
        outSnapshot->ready = !outSnapshot->blocked;
        outSnapshot->attention = input->total > 0U &&
            outSnapshot->percent >= threshold;
        break;
    case UMI_DEVELOPER_OPERATION_CLEANLINESS:
        outSnapshot->ready =
            input->primary == 0U && input->secondary == 0U &&
            !outSnapshot->blocked;
        outSnapshot->attention = !outSnapshot->ready;
        break;
    case UMI_DEVELOPER_OPERATION_CONFLICT:
        outSnapshot->ready = input->secondary == 0U && !outSnapshot->blocked;
        outSnapshot->attention =
            input->secondary > 0U || outSnapshot->blocked;
        break;
    case UMI_DEVELOPER_OPERATION_PROGRESS:
        outSnapshot->ready =
            (input->total == 0U || input->primary >= input->total) &&
            !outSnapshot->blocked;
        outSnapshot->attention = outSnapshot->blocked;
        break;
    case UMI_DEVELOPER_OPERATION_ALERT:
        outSnapshot->attention =
            input->primary > 0U || input->secondary > 0U ||
            outSnapshot->blocked;
        outSnapshot->ready = !outSnapshot->attention;
        break;
    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UmiDeveloperOperationSnapshotValidate(outSnapshot);
}
