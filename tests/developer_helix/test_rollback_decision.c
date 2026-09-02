/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_rollback_decision.c
 *
 * PURPOSE:
 *   Regression coverage for evaluate validation, safety, build and test evidence to choose continue, stop or rollback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/rollback_decision.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ /* Apply this branch only when its contract condition is satisfied. */ if(umi_helix_rollback_decision_evaluate(false,false,false,0.01,0.05)!=UMI_HELIX_DECISION_ALLOW){ return 1; } /* Apply this branch only when its contract condition is satisfied. */ if(umi_helix_rollback_decision_evaluate(false,true,false,0.0,0.05)!=UMI_HELIX_DECISION_ROLLBACK){ return 2; } return 0; }
