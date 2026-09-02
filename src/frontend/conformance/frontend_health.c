/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/frontend_health.c
 *
 * PURPOSE:
 *   aggregate frontend conformance health state and blocker counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/frontend_health.h"

/*
 * Provide the fc frontend health outcome operation used by this module and its client
 * applications.
 */
UmiFcOutcome umi_fc_frontend_health_outcome(const UmiFcFrontendHealth *health){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(health==NULL||health->failed>0U||health->blockers>0U)return UMI_FC_FAIL;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(health->degraded>0U)return UMI_FC_DEGRADED;return UMI_FC_PASS;}
/*
 * Provide the fc frontend health healthy operation used by this module and its client
 * applications.
 */
bool umi_fc_frontend_health_healthy(const UmiFcFrontendHealth *health,double minimum){return health!=NULL&&health->blockers==0U&&health->failed==0U&&health->score>=minimum;}
