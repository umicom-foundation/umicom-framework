/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/responsive_policy.h
 *
 * PURPOSE:
 *   responsive viewport policy selecting compact, standard and wide workstation modes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_RESPONSIVE_POLICY_H
#define UMICOM_FRONTEND_CONFORMANCE_RESPONSIVE_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiFcViewportClass { UMI_FC_VIEW_COMPACT=1,UMI_FC_VIEW_STANDARD=2,UMI_FC_VIEW_WIDE=3,UMI_FC_VIEW_ULTRAWIDE=4 } UmiFcViewportClass;
typedef struct UmiFcResponsivePolicy { int32_t compact_max; int32_t standard_max; int32_t wide_max; } UmiFcResponsivePolicy;
void umi_fc_responsive_policy_default(UmiFcResponsivePolicy *policy);
UmiFcViewportClass umi_fc_responsive_policy_classify(const UmiFcResponsivePolicy *policy,int32_t width);

#ifdef __cplusplus
}
#endif
#endif
