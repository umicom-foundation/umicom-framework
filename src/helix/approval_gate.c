/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/approval_gate.c
 * PURPOSE: Implement exact-scope human approval tickets for Helix v2.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/approval_gate.h"

#include <stdio.h>
#include <string.h>

static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

static UmiHelixApprovalTicket *find_ticket(UmiHelixApprovalGate *gate,
                                             const char *operation_id,
                                             const char *action_id)
{
    size_t index;
    if (gate == NULL || operation_id == NULL || action_id == NULL) return NULL;
    for (index = 0U; index < gate->count; ++index) {
        UmiHelixApprovalTicket *ticket = &gate->tickets[index];
        if (strcmp(ticket->operation_id, operation_id) == 0 &&
            strcmp(ticket->action_id, action_id) == 0) return ticket;
    }
    return NULL;
}

void umi_helix_approval_gate_init(UmiHelixApprovalGate *gate)
{
    if (gate != NULL) (void)memset(gate, 0, sizeof(*gate));
}

UmiStatus umi_helix_approval_gate_request(
    UmiHelixApprovalGate *gate,
    const char *operation_id,
    const char *action_id,
    const char *plan_hash)
{
    UmiHelixApprovalTicket *ticket;
    if (gate == NULL || operation_id == NULL || action_id == NULL ||
        plan_hash == NULL || operation_id[0] == '\0' || action_id[0] == '\0' ||
        plan_hash[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (find_ticket(gate, operation_id, action_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (gate->count >= UMI_HELIX_RUNTIME_MAX_APPROVALS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ticket = &gate->tickets[gate->count];
    (void)memset(ticket, 0, sizeof(*ticket));
    if (!copy_text(ticket->operation_id, sizeof(ticket->operation_id),
                   operation_id) ||
        !copy_text(ticket->action_id, sizeof(ticket->action_id), action_id) ||
        !copy_text(ticket->plan_hash, sizeof(ticket->plan_hash), plan_hash)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ticket->decision = UMI_HELIX_APPROVAL_PENDING;
    ticket->revision = 1U;
    gate->count++;
    return UMI_STATUS_OK;
}

UmiStatus umi_helix_approval_gate_decide(
    UmiHelixApprovalGate *gate,
    const char *operation_id,
    const char *action_id,
    UmiHelixApprovalDecision decision,
    const char *approver,
    const char *reason)
{
    UmiHelixApprovalTicket *ticket = find_ticket(gate, operation_id, action_id);
    if (ticket == NULL) return UMI_STATUS_NOT_FOUND;
    if (decision < UMI_HELIX_APPROVAL_APPROVED ||
        decision > UMI_HELIX_APPROVAL_REVOKED || approver == NULL ||
        approver[0] == '\0' || reason == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (ticket->decision != UMI_HELIX_APPROVAL_PENDING &&
        decision != UMI_HELIX_APPROVAL_REVOKED) return UMI_STATUS_INVALID_STATE;
    if (!copy_text(ticket->approver, sizeof(ticket->approver), approver) ||
        !copy_text(ticket->reason, sizeof(ticket->reason), reason)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ticket->decision = decision;
    ticket->revision++;
    return UMI_STATUS_OK;
}

int umi_helix_approval_gate_is_approved(
    const UmiHelixApprovalGate *gate,
    const char *operation_id,
    const char *action_id,
    const char *plan_hash)
{
    size_t index;
    if (gate == NULL || operation_id == NULL || action_id == NULL ||
        plan_hash == NULL) return 0;
    for (index = 0U; index < gate->count; ++index) {
        const UmiHelixApprovalTicket *ticket = &gate->tickets[index];
        if (strcmp(ticket->operation_id, operation_id) == 0 &&
            strcmp(ticket->action_id, action_id) == 0 &&
            strcmp(ticket->plan_hash, plan_hash) == 0 &&
            ticket->decision == UMI_HELIX_APPROVAL_APPROVED) return 1;
    }
    return 0;
}
