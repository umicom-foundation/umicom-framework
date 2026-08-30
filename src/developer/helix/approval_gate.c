/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/approval_gate.c
 *
 * PURPOSE:
 *   Model explicit human/policy approval required before guarded repository mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/approval_gate.h"
#include <string.h>
/* Initialise conservative policy values so omitted configuration cannot widen autonomy. */
void umi_developer_helix_approval_gate_init(
    UmiDeveloperHelixApprovalGate *g) {
  if (g == NULL) {
    return;
  }
  memset(g, 0, sizeof(*g));
  g->structure_size = (uint32_t)sizeof(*g);
  g->api_version = UMI_HELIX_API_VERSION;
  g->required = true;
  g->approved = false;
}
/* Evaluate gate evidence deterministically. */
UmiHelixDecision umi_developer_helix_approval_gate_evaluate(
    const UmiDeveloperHelixApprovalGate *g) {
  if (g == NULL || g->structure_size != sizeof(*g) || g->api_version != UMI_HELIX_API_VERSION) {
    return UMI_HELIX_DECISION_DENY;
  }
  if (!g->required) {
    return UMI_HELIX_DECISION_ALLOW;
  }
  return g->approved ? UMI_HELIX_DECISION_ALLOW : UMI_HELIX_DECISION_REVIEW;
}
