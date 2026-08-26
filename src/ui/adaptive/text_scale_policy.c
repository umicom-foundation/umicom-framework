/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/text_scale_policy.c
 *
 * PURPOSE:
 *   Clamp user text scaling to accessible, renderer-neutral application limits.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/text_scale_policy.h"

/* Multiply finite positive scales and clamp the result to a safe 0.75x-3.0x range. */
UmiStatus umi_adaptive_text_scale_policy_resolve(double baseline_scale,
                                                 double user_scale,
                                                 double *out_scale)
{
    double value;
    if (out_scale == NULL || !(baseline_scale > 0.0) || !(user_scale > 0.0)) return UMI_STATUS_INVALID_ARGUMENT;
    value = baseline_scale * user_scale;
    if (value < 0.75) value = 0.75;
    if (value > 3.0) value = 3.0;
    *out_scale = value;
    return UMI_STATUS_OK;
}
