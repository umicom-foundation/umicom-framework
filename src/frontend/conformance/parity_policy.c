/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/parity_policy.c
 *
 * PURPOSE:
 *   minimum parity thresholds and blocker policy for release decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/parity_policy.h"

/*
 * Provide the fc parity policy default operation used by this module and its client
 * applications.
 */
void umi_fc_parity_policy_default(UmiFcParityPolicy *policy){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy!=NULL)*policy=(UmiFcParityPolicy){0.92,0.95,0.90,4U,true};}
/*
 * Provide the fc parity policy accepts operation used by this module and its client
 * applications.
 */
bool umi_fc_parity_policy_accepts(const UmiFcParityPolicy *policy,double score,double accessibility,double layout,size_t degraded,size_t blockers){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy==NULL)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy->blockers_forbidden&&blockers>0U)return false;return score>=policy->minimum_score&&accessibility>=policy->minimum_accessibility&&layout>=policy->minimum_layout&&degraded<=policy->maximum_degraded;}
