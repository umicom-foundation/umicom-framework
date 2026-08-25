/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/approval_gate.h
 *
 * PURPOSE:
 *   Model explicit human/policy approval required before guarded repository mutation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_APPROVAL_GATE_H
#define UMICOM_DEVELOPER_HELIX_APPROVAL_GATE_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixApprovalGate { uint32_t structure_size; uint32_t api_version; bool required; bool approved; } UmiHelixApprovalGate;
/* Initialise conservative gate defaults. */
void umi_helix_approval_gate_init(UmiHelixApprovalGate *gate);
/* Evaluate supplied evidence without performing repository mutation. */
UmiHelixDecision umi_helix_approval_gate_evaluate(const UmiHelixApprovalGate *gate);
#ifdef __cplusplus
}
#endif
#endif
