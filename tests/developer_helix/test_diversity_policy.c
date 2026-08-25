/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_diversity_policy.c
 *
 * PURPOSE:
 *   Regression coverage for measure and preserve implementation diversity to avoid repeated equivalent repair candidates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/diversity_policy.h"
int main(void){ UmiHelixDiversityPolicy p={0.1}; if(umi_helix_diversity_policy_distance(0U,0U)!=0.0){ return 1; } if(umi_helix_diversity_policy_accept(&p,0U,0U)){ return 2; } return 0; }
