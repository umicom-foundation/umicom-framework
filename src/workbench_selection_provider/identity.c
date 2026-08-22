/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/identity.c
 *
 * PURPOSE:
 *   Implement deterministic selection identifiers without heap allocation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/identity.h"

#include <stdio.h>

UmiStatus umi_workbench_selection_provider_make_id(
    char *out_id,
    size_t capacity,
    const char *prefix,
    const char *subject_id,
    uint64_t revision)
{
    int written;
    if (out_id == NULL || capacity == 0U ||
        prefix == NULL || subject_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(
        out_id,
        capacity,
        "%s:%s:%llu",
        prefix,
        subject_id,
        (unsigned long long)revision);
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

UmiStatus umi_workbench_selection_provider_make_indexed_id(
    char *out_id,
    size_t capacity,
    const char *prefix,
    size_t index,
    uint64_t revision)
{
    int written;
    if (out_id == NULL || capacity == 0U || prefix == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(
        out_id,
        capacity,
        "%s:%zu:%llu",
        prefix,
        index,
        (unsigned long long)revision);
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}
