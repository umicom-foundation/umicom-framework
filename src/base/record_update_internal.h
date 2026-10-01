/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/record_update_internal.h
 * PURPOSE: Share revision-checked publication for caller-owned value records.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_RECORD_UPDATE_INTERNAL_H
#define UMICOM_RECORD_UPDATE_INTERNAL_H
#include "umicom/base/status.h"
#include <stdint.h>
#include <string.h>

/* Use this only for records with inline value fields, a validated fixed id
 * array and a uint64_t revision. No pointer, resource or callback may acquire
 * a second owner through assignment. Keep validation in the domain: adding a
 * field must extend its validator before it becomes publishable here. */
#define UMI_DEFINE_REVIEWED_RECORD_EDIT(Function, Record, Validate) \
UmiStatus Function(Record *value, uint64_t expected_revision, const Record *proposal) \
{ \
    if (value == NULL || proposal == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    if (value->revision != expected_revision) return UMI_STATUS_INVALID_STATE; \
    if (value->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED; \
    UmiStatus status = Validate(value); \
    if (status != UMI_STATUS_OK) return status; \
    status = Validate(proposal); \
    if (status != UMI_STATUS_OK) return status; \
    /* A reviewed edit belongs to one identity; creating or renaming a record \
     * remains an explicit owner operation. Both IDs are bounded above. */ \
    if (strcmp(value->id, proposal->id) != 0) return UMI_STATUS_INVALID_ARGUMENT; \
    Record staged = *proposal; \
    staged.revision = value->revision + 1U; \
    /* All possible failures precede this assignment. Proposal revision is \
     * intentionally ignored: only the live owner issues observation tokens. */ \
    *value = staged; \
    return UMI_STATUS_OK; \
}
#endif
