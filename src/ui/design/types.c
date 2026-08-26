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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/types.h"

#include <math.h>
#include <string.h>

UmiStatus umi_design_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || source == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

int umi_design_number_valid(double value) { return isfinite(value) ? 1 : 0; }

UmiStatus umi_design_clamp(double value, double minimum, double maximum, double *out_value)
{
    if (out_value == NULL || !isfinite(value) || !isfinite(minimum) || !isfinite(maximum) || minimum > maximum) return UMI_STATUS_INVALID_ARGUMENT;
    *out_value = value < minimum ? minimum : (value > maximum ? maximum : value);
    return UMI_STATUS_OK;
}

uint64_t umi_design_hash_text(const char *text)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    const unsigned char *cursor;
    if (text == NULL) return 0U;
    cursor = (const unsigned char *)text;
    while (*cursor != 0U) { hash ^= (uint64_t)*cursor; hash *= UINT64_C(1099511628211); ++cursor; }
    return hash;
}
