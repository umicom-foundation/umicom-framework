/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/layout_adaptation.h
 *
 * PURPOSE:
 *   explicit desktop-to-web and constrained-viewport layout adaptation planning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_LAYOUT_ADAPTATION_H
#define UMICOM_FRONTEND_CONFORMANCE_LAYOUT_ADAPTATION_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiFcLayoutAdaptationKind { UMI_FC_LAYOUT_KEEP=1,UMI_FC_LAYOUT_COLLAPSE=2,UMI_FC_LAYOUT_TAB=3,UMI_FC_LAYOUT_STACK=4,UMI_FC_LAYOUT_HIDE=5 } UmiFcLayoutAdaptationKind;
typedef struct UmiFcLayoutAdaptation { UmiFcLayoutAdaptationKind kind; uint64_t lost_capabilities; bool user_visible; } UmiFcLayoutAdaptation;
UmiFcLayoutAdaptation umi_fc_layout_adaptation_plan(bool compact,bool docking_available,bool floating_available);

#ifdef __cplusplus
}
#endif
#endif
