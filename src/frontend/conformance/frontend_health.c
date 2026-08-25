/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/frontend_health.c
 *
 * PURPOSE:
 *   aggregate frontend conformance health state and blocker counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/frontend_health.h"

UmiFcOutcome umi_fc_frontend_health_outcome(const UmiFcFrontendHealth *health){if(health==NULL||health->failed>0U||health->blockers>0U)return UMI_FC_FAIL;if(health->degraded>0U)return UMI_FC_DEGRADED;return UMI_FC_PASS;}
bool umi_fc_frontend_health_healthy(const UmiFcFrontendHealth *health,double minimum){return health!=NULL&&health->blockers==0U&&health->failed==0U&&health->score>=minimum;}
