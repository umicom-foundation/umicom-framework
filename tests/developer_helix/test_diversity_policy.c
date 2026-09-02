/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_diversity_policy.c
 *
 * PURPOSE:
 *   Regression coverage for measure and preserve implementation diversity to avoid repeated equivalent repair candidates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/diversity_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixDiversityPolicy p={0.1}; /* Apply this branch only when its contract condition is satisfied. */ if(umi_helix_diversity_policy_distance(0U,0U)!=0.0){ return 1; } /* Apply this branch only when its contract condition is satisfied. */ if(umi_helix_diversity_policy_accept(&p,0U,0U)){ return 2; } return 0; }
