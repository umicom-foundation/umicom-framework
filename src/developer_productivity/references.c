/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/references.c
 *
 * PURPOSE:
 *   Implement bounded symbol-reference storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/references.h"

#include <string.h>

/*
 * Initialise developer reference set from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_reference_set_init(
    UmiDeveloperReferenceSet *set)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL) return;
    (void)memset(set, 0, sizeof(*set));
    set->revision = 1U;
}

/*
 * Add developer reference set only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_developer_reference_set_add(
    UmiDeveloperReferenceSet *set,
    const UmiDeveloperReference *reference)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || reference == NULL ||
        reference->symbol_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_productivity_location_validate(
        &reference->location);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (set->count >= UMI_DEVELOPER_REFERENCE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    set->references[set->count++] = *reference;
    set->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer reference set for symbol operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_reference_set_for_symbol(
    const UmiDeveloperReferenceSet *set,
    const char *symbol_id,
    UmiDeveloperReference *out_references,
    size_t capacity,
    size_t *out_count)
{
    size_t index;
    size_t used = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || symbol_id == NULL ||
        out_references == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < set->count && used < capacity;
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(set->references[index].symbol_id, symbol_id) == 0) {
            out_references[used++] = set->references[index];
        }
    }

    *out_count = used;
    return used > 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
