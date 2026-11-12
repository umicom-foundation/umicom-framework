/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/operational_internal.h
 *
 * PURPOSE:
 *   Share bounded query and finite arithmetic checks for Treasury preview books.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_OPEN_TMS_OPERATIONAL_INTERNAL_H
#define UMICOM_OPEN_TMS_OPERATIONAL_INTERNAL_H
#include "umicom/base/snapshot_validation.h"
#include <math.h>
#include <string.h>

/* Query pointers are readable terminated strings, or capacity-readable arrays.
 * Stop at the terminator instead of reading an entire maximum-sized object. */
static inline int TmsOpsQuery(const char *text, size_t capacity)
{
    if (text == NULL) return 0;
    for (size_t i = 0U; i < capacity; ++i)
        if (text[i] == '\0') return i != 0U;
    return 0;
}
/* Preview arithmetic keeps signed finite observations. A failed intermediate
 * does not become a usable zero or a partially published report. */
static inline UmiStatus TmsOpsAdd(double a, double b, double *out)
{
    double value = a + b;
    if (!isfinite(value)) return UMI_STATUS_CAPACITY_EXCEEDED;
    *out = value;
    return UMI_STATUS_OK;
}
static inline UmiStatus TmsOpsMultiply(double a, double b, double *out)
{
    double value = a * b;
    if (!isfinite(value)) return UMI_STATUS_CAPACITY_EXCEEDED;
    *out = value;
    return UMI_STATUS_OK;
}
#endif
