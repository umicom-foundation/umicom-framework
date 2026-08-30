/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/approval_gate.h
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
#ifndef UMICOM_DEVELOPER_HELIX_APPROVAL_GATE_H
#define UMICOM_DEVELOPER_HELIX_APPROVAL_GATE_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Namespace the developer policy model away from the runtime ticket gate. */
typedef struct UmiDeveloperHelixApprovalGate {
  uint32_t structure_size;
  uint32_t api_version;
  bool required;
  bool approved;
} UmiDeveloperHelixApprovalGate;
/* Initialise conservative gate defaults. */
void umi_developer_helix_approval_gate_init(
    UmiDeveloperHelixApprovalGate *gate);
/* Evaluate supplied evidence without performing repository mutation. */
UmiHelixDecision umi_developer_helix_approval_gate_evaluate(
    const UmiDeveloperHelixApprovalGate *gate);
#ifdef __cplusplus
}
#endif
#endif
