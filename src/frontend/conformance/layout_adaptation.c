/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/layout_adaptation.c
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
#include "umicom/frontend/conformance/layout_adaptation.h"

UmiFcLayoutAdaptation umi_fc_layout_adaptation_plan(bool compact,bool docking_available,bool floating_available){UmiFcLayoutAdaptation a={UMI_FC_LAYOUT_KEEP,0U,false};if(compact){a.kind=UMI_FC_LAYOUT_STACK;a.user_visible=true;}if(!docking_available){a.kind=UMI_FC_LAYOUT_TAB;a.lost_capabilities|=UINT64_C(1)<<UMI_FC_CAP_DOCK;a.user_visible=true;}if(!floating_available)a.lost_capabilities|=UINT64_C(1)<<UMI_FC_CAP_FLOAT;return a;}
