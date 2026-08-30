/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_validation_gate.c
 *
 * PURPOSE:
 *   Regression coverage for define one validation requirement with severity, threshold and blocking semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/validation_gate.h"
int main(void){ UmiHelixValidationGate r; umi_helix_validation_gate_init(&r,"validation_gate"); if(umi_helix_validation_gate_validate(&r)!=UMI_STATUS_OK){ return 1; } r.threshold=0.8; r.blocking=true; double s=umi_helix_validation_gate_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
