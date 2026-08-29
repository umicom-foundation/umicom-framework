/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_approval_gate.c
 *
 * PURPOSE:
 *   Regression coverage for model explicit human/policy approval required before guarded repository
 *mutation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/approval_gate.h"
int main(void) {
  UmiDeveloperHelixApprovalGate g;
  umi_developer_helix_approval_gate_init(&g);
  g.required = true;
  g.approved = false;
  if (umi_developer_helix_approval_gate_evaluate(&g) !=
      UMI_HELIX_DECISION_REVIEW) {
    return 1;
  }
  g.approved = true;
  if (umi_developer_helix_approval_gate_evaluate(&g) !=
      UMI_HELIX_DECISION_ALLOW) {
    return 2;
  }
  return 0;
}
