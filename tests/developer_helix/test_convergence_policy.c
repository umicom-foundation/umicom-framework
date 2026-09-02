/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_convergence_policy.c
 *
 * PURPOSE:
 *   Regression coverage for determine whether evolution should stop due to target fitness, stagnation, budget or generation limits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/convergence_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixConvergencePolicy p={0.9,3,10,100}; /* Apply this branch only when its contract condition is satisfied. */ if(umi_helix_convergence_policy_evaluate(&p,0.95,0,1,1)!=UMI_HELIX_CONVERGENCE_TARGET){ return 1; } /* Apply this branch only when its contract condition is satisfied. */ if(umi_helix_convergence_policy_evaluate(&p,0.5,0,1,1)!=UMI_HELIX_CONVERGENCE_CONTINUE){ return 2; } return 0; }
