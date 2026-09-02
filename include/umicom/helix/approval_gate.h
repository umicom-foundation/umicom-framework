/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/approval_gate.h
 * PURPOSE: Bind human decisions to exact Helix v2 plans and actions.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Approval is not a global switch.  It authorises one action
 * under one immutable plan hash and can be rejected or revoked explicitly. */
#ifndef INCLUDE_UMICOM_HELIX_APPROVAL_GATE_H
#define INCLUDE_UMICOM_HELIX_APPROVAL_GATE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/helix/types.h"
#include "umicom/helix/execution_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named helix approval decision values accepted by this public contract.
 */
typedef enum UmiHelixApprovalDecision {
    UMI_HELIX_APPROVAL_PENDING = 1,
    UMI_HELIX_APPROVAL_APPROVED = 2,
    UMI_HELIX_APPROVAL_REJECTED = 3,
    UMI_HELIX_APPROVAL_REVOKED = 4
} UmiHelixApprovalDecision;

/**
 * Represent the helix approval ticket data shared with callers of this public contract.
 */
typedef struct UmiHelixApprovalTicket {
    char operation_id[UMI_HELIX_ID_CAPACITY];
    char action_id[UMI_HELIX_ID_CAPACITY];
    char plan_hash[UMI_HELIX_RUNTIME_HASH_CAPACITY];
    char approver[UMI_HELIX_ID_CAPACITY];
    char reason[UMI_HELIX_TEXT_CAPACITY];
    UmiHelixApprovalDecision decision;
    uint64_t revision;
} UmiHelixApprovalTicket;

/**
 * Represent the helix approval gate data shared with callers of this public contract.
 */
typedef struct UmiHelixApprovalGate {
    UmiHelixApprovalTicket tickets[UMI_HELIX_RUNTIME_MAX_APPROVALS];
    size_t count;
} UmiHelixApprovalGate;

/**
 * Initialise helix approval gate from caller-provided values so later operations receive a
 * known state.
 */
void umi_helix_approval_gate_init(UmiHelixApprovalGate *gate);
/**
 * Provide the helix approval gate request operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_approval_gate_request(
    UmiHelixApprovalGate *gate,
    const char *operation_id,
    const char *action_id,
    const char *plan_hash);
/**
 * Provide the helix approval gate decide operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_approval_gate_decide(
    UmiHelixApprovalGate *gate,
    const char *operation_id,
    const char *action_id,
    UmiHelixApprovalDecision decision,
    const char *approver,
    const char *reason);
/**
 * Provide the helix approval gate is approved operation used by this module and its client
 * applications.
 */
int umi_helix_approval_gate_is_approved(
    const UmiHelixApprovalGate *gate,
    const char *operation_id,
    const char *action_id,
    const char *plan_hash);

#ifdef __cplusplus
}
#endif

#endif
