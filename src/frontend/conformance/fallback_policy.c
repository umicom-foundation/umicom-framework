/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/fallback_policy.c
 *
 * PURPOSE:
 *   canonical fallback selection for unavailable frontend capabilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/fallback_policy.h"

UmiFcFallbackKind umi_fc_fallback_policy_for(UmiFcCapabilityKind capability,UmiFcFrontendKind frontend){if(frontend==UMI_FC_FRONTEND_NATIVE_WEB){if(capability==UMI_FC_CAP_FLOAT||capability==UMI_FC_CAP_MULTI_MONITOR)return UMI_FC_FALLBACK_TAB;if(capability==UMI_FC_CAP_AUTO_HIDE)return UMI_FC_FALLBACK_STACK;}if(frontend==UMI_FC_FRONTEND_HEADLESS)return UMI_FC_FALLBACK_STATIC;return UMI_FC_FALLBACK_NONE;}
bool umi_fc_fallback_policy_preserves_semantics(UmiFcFallbackKind fallback){return fallback==UMI_FC_FALLBACK_TAB||fallback==UMI_FC_FALLBACK_STACK||fallback==UMI_FC_FALLBACK_DIALOG||fallback==UMI_FC_FALLBACK_SERVER_DRIVEN;}
