/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_approval_gate.c
 *
 * PURPOSE:
 *   Regression coverage for model explicit human/policy approval required before guarded repository
 *mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/approval_gate.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  UmiDeveloperHelixApprovalGate g;
  umi_developer_helix_approval_gate_init(&g);
  g.required = true;
  g.approved = false;
  /* Apply this branch only when its contract condition is satisfied. */
  if (umi_developer_helix_approval_gate_evaluate(&g) !=
      UMI_HELIX_DECISION_REVIEW) {
    return 1;
  }
  g.approved = true;
  /* Apply this branch only when its contract condition is satisfied. */
  if (umi_developer_helix_approval_gate_evaluate(&g) !=
      UMI_HELIX_DECISION_ALLOW) {
    return 2;
  }
  return 0;
}
