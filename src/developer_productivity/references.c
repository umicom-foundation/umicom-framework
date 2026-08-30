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

void umi_developer_reference_set_init(
    UmiDeveloperReferenceSet *set)
{
    if (set == NULL) return;
    (void)memset(set, 0, sizeof(*set));
    set->revision = 1U;
}

UmiStatus umi_developer_reference_set_add(
    UmiDeveloperReferenceSet *set,
    const UmiDeveloperReference *reference)
{
    UmiStatus status;

    if (set == NULL || reference == NULL ||
        reference->symbol_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_productivity_location_validate(
        &reference->location);
    if (status != UMI_STATUS_OK) return status;

    if (set->count >= UMI_DEVELOPER_REFERENCE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    set->references[set->count++] = *reference;
    set->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_reference_set_for_symbol(
    const UmiDeveloperReferenceSet *set,
    const char *symbol_id,
    UmiDeveloperReference *out_references,
    size_t capacity,
    size_t *out_count)
{
    size_t index;
    size_t used = 0U;

    if (set == NULL || symbol_id == NULL ||
        out_references == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U;
         index < set->count && used < capacity;
         ++index) {
        if (strcmp(set->references[index].symbol_id, symbol_id) == 0) {
            out_references[used++] = set->references[index];
        }
    }

    *out_count = used;
    return used > 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
