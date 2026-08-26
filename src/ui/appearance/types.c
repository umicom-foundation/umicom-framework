/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/types.c
 *
 * PURPOSE:
 *   Define stable appearance identifiers, theme scopes, renderer kinds and bounded utility operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/types.h"
#include <math.h>
#include <string.h>

/* Copy text while preserving deterministic bounded public ABI storage. */
UmiStatus umi_appearance_copy_text(char *destination, size_t capacity, const char *source) {
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Validate identifiers before they enter catalogues, graphs or persistence records. */
int umi_appearance_id_valid(const char *identifier) {
    if (identifier == NULL || identifier[0] == '\0') return 0;
    return strlen(identifier) < UMI_APPEARANCE_ID_CAPACITY;
}

/* Clamp scale factors used for DPI, text and user accessibility transforms. */
UmiStatus umi_appearance_clamp_scale(double value, double minimum, double maximum, double *out_value) {
    if (out_value == NULL || !isfinite(value) || !isfinite(minimum) || !isfinite(maximum) || minimum > maximum) return UMI_STATUS_INVALID_ARGUMENT;
    if (value < minimum) value = minimum;
    if (value > maximum) value = maximum;
    *out_value = value;
    return UMI_STATUS_OK;
}
