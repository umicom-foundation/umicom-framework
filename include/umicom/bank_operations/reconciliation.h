/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_operations/reconciliation.h
 * PURPOSE: Expose retained reconciliation evidence and explain reviewed break transitions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_OPERATIONS_RECONCILIATION_H
#define UMICOM_BANK_OPERATIONS_RECONCILIATION_H
#include "umicom/bank_operations/operations.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Find a copied reconciliation by exact ID. Output is zero on failure. */
UmiStatus UmiBankOperationsFindReconciliation(const UmiBankOperations *operations,
    const char *id, UmiBankReconciliation *outRecord);
/** Stable English label: Matched, Open break, Resolved break or Reopened break.
 * NULL or an unknown disposition returns Unknown. No state is changed. */
const char *UmiBankReconciliationStateName(const UmiBankReconciliation *record);
/** Resolve uses command.id as the original unmatched comparison, ownerId as
 * a later matching comparison for that account, and name as a nonblank reason
 * of at most UMI_FINANCE_NAME_CAPACITY-1 bytes.
 * It requires OPERATE and the latest comparison for that account. The evidence
 * must be later than the break's creation or latest disposition, and no journal
 * may affect that account after the evidence. Current booked money must match.
 * Reopen uses id and a nonblank name, only for a resolved break; a later resolve
 * requires fresh matching evidence after the reopen. Neither action posts,
 * reserves, transfers, changes original comparison facts or erases history.
 *
 * Submit through UmiBankOperationsReview/ExecuteReviewed for an explicit
 * preview. Expected revision, capabilities, history, idempotency and storage
 * transactions follow the existing operations service contract. These local
 * comparisons are caller-entered observations, not verified bank statements.
 * Blocked/closed accounts may be investigated because no money is moved. */
#ifdef __cplusplus
}
#endif
#endif
