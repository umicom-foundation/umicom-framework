/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_safety_gate.c
 *
 * PURPOSE:
 *   Regression coverage for block changes that exceed scope, destructive-operation or risk constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/safety_gate.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixSafetyGate g; umi_helix_safety_gate_init(&g); g.maximum_risk=0.4; g.allow_destructive=false; /* Apply this branch only when its contract condition is satisfied. */ if(umi_helix_safety_gate_evaluate(&g,0.2,false)!=UMI_HELIX_DECISION_ALLOW||umi_helix_safety_gate_evaluate(&g,0.8,false)!=UMI_HELIX_DECISION_DENY){ return 1; } return 0; }
