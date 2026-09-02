/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/fallback_policy.c
 *
 * PURPOSE:
 *   canonical fallback selection for unavailable frontend capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/fallback_policy.h"

/*
 * Provide the fc fallback policy for operation used by this module and its client
 * applications.
 */
UmiFcFallbackKind umi_fc_fallback_policy_for(UmiFcCapabilityKind capability,UmiFcFrontendKind frontend){/* Apply this branch only when its contract condition is satisfied. */ if(frontend==UMI_FC_FRONTEND_NATIVE_WEB){/* Apply this branch only when its contract condition is satisfied. */ if(capability==UMI_FC_CAP_FLOAT||capability==UMI_FC_CAP_MULTI_MONITOR)return UMI_FC_FALLBACK_TAB;/* Apply this branch only when its contract condition is satisfied. */ if(capability==UMI_FC_CAP_AUTO_HIDE)return UMI_FC_FALLBACK_STACK;}/* Apply this branch only when its contract condition is satisfied. */ if(frontend==UMI_FC_FRONTEND_HEADLESS)return UMI_FC_FALLBACK_STATIC;return UMI_FC_FALLBACK_NONE;}
/*
 * Provide the fc fallback policy preserves semantics operation used by this module and its
 * client applications.
 */
bool umi_fc_fallback_policy_preserves_semantics(UmiFcFallbackKind fallback){return fallback==UMI_FC_FALLBACK_TAB||fallback==UMI_FC_FALLBACK_STACK||fallback==UMI_FC_FALLBACK_DIALOG||fallback==UMI_FC_FALLBACK_SERVER_DRIVEN;}
