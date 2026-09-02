/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/types.c
 *
 * PURPOSE:
 *   Define shared toolkit-neutral design-system values, limits and utility operations.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/types.h"

#include <math.h>
#include <string.h>

/* Provide the design copy text operation used by this module and its client applications. */
UmiStatus umi_design_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Check that design number satisfies its contract before another service relies on it. */
int umi_design_number_valid(double value) { return isfinite(value) ? 1 : 0; }

/* Provide the design clamp operation used by this module and its client applications. */
UmiStatus umi_design_clamp(double value, double minimum, double maximum, double *out_value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_value == NULL || !isfinite(value) || !isfinite(minimum) || !isfinite(maximum) || minimum > maximum) return UMI_STATUS_INVALID_ARGUMENT;
    *out_value = value < minimum ? minimum : (value > maximum ? maximum : value);
    return UMI_STATUS_OK;
}

/* Provide the design hash text operation used by this module and its client applications. */
uint64_t umi_design_hash_text(const char *text)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    const unsigned char *cursor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0U;
    cursor = (const unsigned char *)text;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != 0U) { hash ^= (uint64_t)*cursor; hash *= UINT64_C(1099511628211); ++cursor; }
    return hash;
}
