/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/density_policy.h
 *
 * PURPOSE:
 *   Choose canonical Design-System density from viewport and input capabilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_DENSITY_POLICY_H
#define UMICOM_UI_ADAPTIVE_DENSITY_POLICY_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Resolve the existing canonical Design-System density for an adaptive context. */
UmiStatus umi_adaptive_density_policy_resolve(UmiDesignSizeClass size_class,
                                              UmiAdaptiveInputMode input_mode,
                                              UmiDesignDensity *out_density);

#ifdef __cplusplus
}
#endif
#endif
