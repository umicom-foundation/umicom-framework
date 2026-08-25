/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/parity_policy.c
 *
 * PURPOSE:
 *   minimum parity thresholds and blocker policy for release decisions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/parity_policy.h"

void umi_fc_parity_policy_default(UmiFcParityPolicy *policy){if(policy!=NULL)*policy=(UmiFcParityPolicy){0.92,0.95,0.90,4U,true};}
bool umi_fc_parity_policy_accepts(const UmiFcParityPolicy *policy,double score,double accessibility,double layout,size_t degraded,size_t blockers){if(policy==NULL)return false;if(policy->blockers_forbidden&&blockers>0U)return false;return score>=policy->minimum_score&&accessibility>=policy->minimum_accessibility&&layout>=policy->minimum_layout&&degraded<=policy->maximum_degraded;}
