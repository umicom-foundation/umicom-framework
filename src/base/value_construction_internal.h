/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/value_construction_internal.h
 * PURPOSE: Publish inline values only after their constructor accepts every input.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VALUE_CONSTRUCTION_INTERNAL_H
#define UMICOM_VALUE_CONSTRUCTION_INTERNAL_H
#include "umicom/base/status.h"
#include <stdlib.h>
#include <string.h>

/* Bind only value types containing inline fields, never handles, owning pointers
 * or callbacks. Build must affect only its candidate and must release anything
 * it acquires on failure. Existing domain defaults and error codes stay in Build.
 *
 * Allocate the candidate off the call stack: some public metadata values contain
 * bounded arrays large enough to exhaust a Windows thread stack. Allocation
 * failure leaves the destination unchanged. Inputs may refer into the previous
 * destination, because publication happens after Build has finished reading.
 * Callers still serialize access; this is failure isolation, not a lock. */
#define UMI_DEFINE_STAGED_VALUE_INIT(Function, Record, Destination, Parameters, BuildCall) \
UmiStatus Function Parameters \
{ \
    if (Destination == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    Record *candidate = malloc(sizeof(*candidate)); \
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY; \
    UmiStatus construction_status = BuildCall; \
    if (construction_status == UMI_STATUS_OK) { \
        /* The retained builder initializes the complete value before accepting it. */ \
        memcpy(Destination, candidate, sizeof(*candidate)); \
    } \
    free(candidate); \
    return construction_status; \
}
#endif
