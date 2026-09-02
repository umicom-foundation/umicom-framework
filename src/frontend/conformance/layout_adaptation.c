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

/*
 * Provide the fc layout adaptation plan operation used by this module and its client
 * applications.
 */
UmiFcLayoutAdaptation umi_fc_layout_adaptation_plan(bool compact,bool docking_available,bool floating_available){UmiFcLayoutAdaptation a={UMI_FC_LAYOUT_KEEP,0U,false};/* Apply this operation only while the related capability or state is available. */ if(compact){a.kind=UMI_FC_LAYOUT_STACK;a.user_visible=true;}/* Apply this operation only while the related capability or state is available. */ if(!docking_available){a.kind=UMI_FC_LAYOUT_TAB;a.lost_capabilities|=UINT64_C(1)<<UMI_FC_CAP_DOCK;a.user_visible=true;}/* Apply this operation only while the related capability or state is available. */ if(!floating_available)a.lost_capabilities|=UINT64_C(1)<<UMI_FC_CAP_FLOAT;return a;}
