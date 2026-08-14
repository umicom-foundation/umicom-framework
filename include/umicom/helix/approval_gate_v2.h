/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/approval_gate_v2.h
 * PURPOSE: Bind human decisions to exact Helix v2 plans and actions.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Approval is not a global switch.  It authorises one action
 * under one immutable plan hash and can be rejected or revoked explicitly. */
#ifndef INCLUDE_UMICOM_HELIX_APPROVAL_GATE_V2_H
#define INCLUDE_UMICOM_HELIX_APPROVAL_GATE_V2_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/helix/types.h"
#include "umicom/helix/v2_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiHelixApprovalDecisionV2 {
    UMI_HELIX_APPROVAL_PENDING = 1,
    UMI_HELIX_APPROVAL_APPROVED = 2,
    UMI_HELIX_APPROVAL_REJECTED = 3,
    UMI_HELIX_APPROVAL_REVOKED = 4
} UmiHelixApprovalDecisionV2;

typedef struct UmiHelixApprovalTicketV2 {
    char operation_id[UMI_HELIX_ID_CAPACITY];
    char action_id[UMI_HELIX_ID_CAPACITY];
    char plan_hash[UMI_HELIX_V2_HASH_CAPACITY];
    char approver[UMI_HELIX_ID_CAPACITY];
    char reason[UMI_HELIX_TEXT_CAPACITY];
    UmiHelixApprovalDecisionV2 decision;
    uint64_t revision;
} UmiHelixApprovalTicketV2;

typedef struct UmiHelixApprovalGateV2 {
    UmiHelixApprovalTicketV2 tickets[UMI_HELIX_V2_MAX_APPROVALS];
    size_t count;
} UmiHelixApprovalGateV2;

void umi_helix_approval_gate_v2_init(UmiHelixApprovalGateV2 *gate);
UmiStatus umi_helix_approval_gate_v2_request(
    UmiHelixApprovalGateV2 *gate,
    const char *operation_id,
    const char *action_id,
    const char *plan_hash);
UmiStatus umi_helix_approval_gate_v2_decide(
    UmiHelixApprovalGateV2 *gate,
    const char *operation_id,
    const char *action_id,
    UmiHelixApprovalDecisionV2 decision,
    const char *approver,
    const char *reason);
int umi_helix_approval_gate_v2_is_approved(
    const UmiHelixApprovalGateV2 *gate,
    const char *operation_id,
    const char *action_id,
    const char *plan_hash);

#ifdef __cplusplus
}
#endif

#endif
