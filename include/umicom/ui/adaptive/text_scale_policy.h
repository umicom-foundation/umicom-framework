/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/text_scale_policy.h
 *
 * PURPOSE:
 *   Clamp user text scaling to accessible, renderer-neutral application limits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_TEXT_SCALE_POLICY_H
#define UMICOM_UI_ADAPTIVE_TEXT_SCALE_POLICY_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Resolve final text scale from design baseline and user accessibility preference. */
UmiStatus umi_adaptive_text_scale_policy_resolve(double baseline_scale,
                                                 double user_scale,
                                                 double *out_scale);

#ifdef __cplusplus
}
#endif
#endif
