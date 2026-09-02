/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_quality_gate.c
 *
 * PURPOSE:
 *   Regression coverage for apply quality-score and regression thresholds before an engineering result may advance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/quality_gate.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixQualityGate g; umi_helix_quality_gate_init(&g); g.minimum_score=0.8; g.maximum_regression=0.05; /* Apply this branch only when its contract condition is satisfied. */ if(umi_helix_quality_gate_evaluate(&g,0.9,0.01)!=UMI_HELIX_DECISION_ALLOW){ return 1; } return 0; }
