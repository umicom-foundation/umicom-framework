/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/control_query.h
 *
 * PURPOSE:
 *   Admit bounded financial-control query identities through the shared text contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_FINANCIAL_PLATFORM_CONTROL_QUERY_H
#define UMICOM_FINANCIAL_PLATFORM_CONTROL_QUERY_H
#include "umicom/base/text.h"
#include "umicom/base/snapshot_validation.h"
#include <math.h>
#include <string.h>
/* Preserve exact identity bytes. Do not truncate a key into a different record
 * or scan beyond its declared ABI capacity. Source is terminated readable text,
 * or has capacity readable bytes; destination is separate local storage. */
static inline UmiStatus FinancialControlQuery(const char *source, char *destination, size_t capacity)
{
    UmiStatus status = umi_text_replace(destination, capacity, source);
    if (status != UMI_STATUS_OK) return status;
    return destination[0] == '\0' ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
}
#endif
