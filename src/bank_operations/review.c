/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/review.c
 * PURPOSE: Own a review of the canonical banking state transition.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/bank_operations/review.h"
#include <stdlib.h>
#include <string.h>

struct UmiBankReview {
    UmiBankReviewSnapshot snapshot;
    UmiBankAuditEvent *history;
};

/* Canonical event comparison now belongs to one internal Framework helper shared by command reviews and retained work queues. The previous implementation remains for engineering review. */
#if 0
static UmiStatus SameEvent(const UmiBankAuditEvent *a, const UmiBankAuditEvent *b, bool *same)
{
    char left[BANK_RECORD_TEXT_CAPACITY], right[BANK_RECORD_TEXT_CAPACITY];
    UmiStatus status = BankEncode(a, left, sizeof left);
    *same = false;
    if (status == UMI_STATUS_OK) status = BankEncode(b, right, sizeof right);
    if (status == UMI_STATUS_OK) *same = strcmp(left, right) == 0;
    return status;
}
#endif
static UmiStatus SameEvent(const UmiBankAuditEvent *a,const UmiBankAuditEvent *b,bool *same)
{
    return BankEventSame(a,b,same);
}

static bool SameBalance(const UmiBankBalance *a, const UmiBankBalance *b)
{
    return a->booked.minor_units == b->booked.minor_units &&
        a->reserved.minor_units == b->reserved.minor_units &&
        a->available.minor_units == b->available.minor_units &&
        a->booked.scale == b->booked.scale &&
        umi_accounting_currency_equal(a->booked.currency, b->booked.currency);
}

static UmiStatus ProjectAccounts(const BankState *before, const BankState *after,
    UmiBankReviewSnapshot *snapshot)
{
    for (size_t i = 0U; i < after->counts.accounts; ++i) {
        UmiBankReviewAccount row = {0};
        int oldIndex;
        UmiStatus status;
        row.accountId = after->accounts[i].account.account_id;
        oldIndex = BankFindAccount(before, row.accountId.value);
        row.existedBefore = oldIndex >= 0;
        row.existedAfter = true; /* Canonical transitions retain closed records. */
        row.activeBefore = oldIndex >= 0 && BankAccountActive(before, oldIndex) == UMI_STATUS_OK;
        row.activeAfter = BankAccountActive(after, (int)i) == UMI_STATUS_OK;
        if (row.existedBefore) {
            status = BankProjectBalance(before, row.accountId.value, &row.before);
            if (status != UMI_STATUS_OK) return status;
        }
        status = BankProjectBalance(after, row.accountId.value, &row.after);
        if (status != UMI_STATUS_OK) return status;
        if (!row.existedBefore || row.activeBefore != row.activeAfter || !SameBalance(&row.before, &row.after)) {
            if (snapshot->accountCount >= UMI_BANK_RECORD_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
            snapshot->accounts[snapshot->accountCount++] = row;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiBankOperationsReview(const UmiBankOperations *operations,
    const UmiBankActor *actor, const UmiBankCommand *command, UmiBankReview **outReview)
{
    UmiBankReview *review;
    BankState *candidate = NULL;
    UmiBankReceipt receipt;
    UmiStatus status;
    const BankState *before, *after;
    if (outReview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReview = NULL;
    status = BankPrepare(operations, actor, command, &candidate, &receipt);
    if (status != UMI_STATUS_OK) return status;
    review = calloc(1U, sizeof *review);
    if (review == NULL) { free(candidate); return UMI_STATUS_OUT_OF_MEMORY; }
    before = operations->state;
    after = candidate != NULL ? candidate : before;
    review->snapshot.actor = *actor;
    review->snapshot.command = *command;
    review->snapshot.before = before->counts;
    review->snapshot.after = after->counts;
    review->snapshot.alreadyRecorded = receipt.idempotent;
    review->snapshot.receiptRevision = receipt.idempotent ? receipt.revision : 0U;
    if (before->counts.events != 0U) {
        review->history = malloc(before->counts.events * sizeof *review->history);
        if (review->history == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
        else memcpy(review->history, before->events, before->counts.events * sizeof *review->history);
    }
    if (status == UMI_STATUS_OK) status = ProjectAccounts(before, after, &review->snapshot);
    /* A release/void command carries only an ID. Copy resolved hold economics
     * into the common review so every frontend can explain the same transition. */
    if (status == UMI_STATUS_OK && (command->action == UMI_BANK_HOLD_PLACE ||
        command->action == UMI_BANK_HOLD_RELEASE || (command->action >= UMI_BANK_CARD_AUTHORISE &&
        command->action <= UMI_BANK_CARD_REFUND))) {
        int oldIndex = BankFindHold(before, command->id.value);
        int newIndex = BankFindHold(after, command->id.value);
        if (newIndex < 0) status = UMI_STATUS_INVALID_STATE;
        else {
            review->snapshot.hasHold = true;
            review->snapshot.holdExistedBefore = oldIndex >= 0;
            if (oldIndex >= 0) review->snapshot.holdBefore = before->holds[oldIndex];
            review->snapshot.holdAfter = after->holds[newIndex];
        }
    }
    /* Approval reviews need the resolved economics even without a new journal.
     * A beneficiary reference alone is not a complete description of a payment. */
    if (status == UMI_STATUS_OK && command->action >= UMI_BANK_TRANSFER_SUBMIT &&
        command->action <= UMI_BANK_TRANSFER_REVERSE) {
        int oldIndex = BankFindTransfer(before, command->id.value);
        int newIndex = BankFindTransfer(after, command->id.value);
        if (newIndex < 0) status = UMI_STATUS_INVALID_STATE;
        else {
            review->snapshot.hasTransfer = true;
            review->snapshot.transferExistedBefore = oldIndex >= 0;
            if (oldIndex >= 0) review->snapshot.transferBefore = before->transfers[oldIndex];
            review->snapshot.transferAfter = after->transfers[newIndex];
        }
    }
    /* Approval must show the fixed principal, terms and amount captured by the
     * maker. Current balance changes never silently reprice an approved request. */
    if (status == UMI_STATUS_OK && command->action >= UMI_BANK_INTEREST_SUBMIT &&
        command->action <= UMI_BANK_INTEREST_REVERSE) {
        int oldIndex = BankFindInterest(before, command->id.value);
        int newIndex = BankFindInterest(after, command->id.value);
        if (newIndex < 0) status = UMI_STATUS_INVALID_STATE;
        else {
            review->snapshot.hasInterest = true;
            review->snapshot.interestExistedBefore = oldIndex >= 0;
            if (oldIndex >= 0) review->snapshot.interestBefore = before->interestRequests[oldIndex];
            review->snapshot.interestAfter = after->interestRequests[newIndex];
        }
    }
    /* Later charge actions have an empty payload. Resolve the immutable
     * submitted amount, account and reason so approval is always informed. */
    if (status == UMI_STATUS_OK && command->action >= UMI_BANK_CHARGE_SUBMIT &&
        command->action <= UMI_BANK_CHARGE_REVERSE) {
        int oldIndex = BankFindCharge(before, command->id.value);
        int newIndex = BankFindCharge(after, command->id.value);
        if (newIndex < 0) status = UMI_STATUS_INVALID_STATE;
        else {
            review->snapshot.hasCharge = true;
            review->snapshot.chargeExistedBefore = oldIndex >= 0;
            if (oldIndex >= 0) review->snapshot.chargeBefore = before->chargeRequests[oldIndex];
            review->snapshot.chargeAfter = after->chargeRequests[newIndex];
        }
    }
    if (status == UMI_STATUS_OK && after->counts.journals > before->counts.journals) {
        if (after->counts.journals != before->counts.journals + 1U) status = UMI_STATUS_INVALID_STATE;
        else {
            review->snapshot.hasJournal = true;
            review->snapshot.journal = after->journals[before->counts.journals];
        }
    }
    if (status == UMI_STATUS_OK && after->counts.reconciliations > before->counts.reconciliations) {
        review->snapshot.hasReconciliation = true;
        review->snapshot.reconciliation = after->reconciliations[before->counts.reconciliations];
    }
    /* Resolve/reopen retains the record count, so count growth alone cannot
     * describe this transition. Evidence is copied into the owned review. */
    if (status == UMI_STATUS_OK && (command->action == UMI_BANK_RECONCILIATION_RESOLVE ||
        command->action == UMI_BANK_RECONCILIATION_REOPEN)) {
        int oldIndex = BankFindReconciliation(before, command->id.value);
        int newIndex = BankFindReconciliation(after, command->id.value);
        if (oldIndex < 0 || newIndex < 0) status = UMI_STATUS_INVALID_STATE;
        else {
            review->snapshot.hasReconciliation = true;
            review->snapshot.reconciliationExistedBefore = true;
            review->snapshot.reconciliationBefore = before->reconciliations[oldIndex];
            review->snapshot.reconciliation = after->reconciliations[newIndex];
            const char *evidenceId = command->action == UMI_BANK_RECONCILIATION_RESOLVE ?
                command->ownerId.value : after->reconciliations[newIndex].evidenceId.value;
            int evidenceIndex = BankFindReconciliation(after, evidenceId);
            if (evidenceIndex >= 0) {
                review->snapshot.hasReconciliationEvidence = true;
                review->snapshot.reconciliationEvidence = after->reconciliations[evidenceIndex];
            }
        }
    }
    free(candidate);
    if (status != UMI_STATUS_OK) { UmiBankReviewDestroy(review); return status; }
    *outReview = review;
    return UMI_STATUS_OK;
}

void UmiBankReviewDestroy(UmiBankReview *review)
{
    if (review == NULL) return;
    free(review->history);
    free(review);
}

UmiStatus UmiBankReviewSnapshotRead(const UmiBankReview *review, UmiBankReviewSnapshot *out)
{
    if (out != NULL) memset(out, 0, sizeof *out);
    if (review == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = review->snapshot;
    return UMI_STATUS_OK;
}

UmiStatus UmiBankReviewMatches(const UmiBankReview *review,
    const UmiBankActor *actor, const UmiBankCommand *command, bool *outMatches)
{
    UmiBankAuditEvent original = {0}, supplied = {0};
    UmiStatus status;
    if (outMatches != NULL) *outMatches = false;
    if (review == NULL || outMatches == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = BankCommandValid(actor, command);
    if (status != UMI_STATUS_OK) return status;
    if (command->expectedRevision != review->snapshot.command.expectedRevision) return UMI_STATUS_OK;
    original.actor = review->snapshot.actor;
    original.command = review->snapshot.command;
    supplied.actor = *actor;
    supplied.command = *command;
    return SameEvent(&original, &supplied, outMatches);
}

UmiStatus UmiBankOperationsExecuteReviewed(UmiBankOperations *operations,
    const UmiBankActor *actor, const UmiBankReview *review, UmiBankReceipt *outReceipt)
{
    bool same = false;
    UmiStatus status;
    if (outReceipt != NULL) memset(outReceipt, 0, sizeof *outReceipt);
    if (operations == NULL || review == NULL || outReceipt == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (operations->poisoned || operations->state == NULL) return UMI_STATUS_INVALID_STATE;
    status = UmiBankReviewMatches(review, actor, &review->snapshot.command, &same);
    if (status != UMI_STATUS_OK) return status;
    if (!same) return UMI_STATUS_PERMISSION_DENIED;
/* Reviewed execution and queue selection share complete history comparison, including equivalent-revision histories from different stores. The previous implementation remains for engineering review. */
#if 0
    if (operations->state->counts.revision != review->snapshot.before.revision ||
        operations->state->counts.events != review->snapshot.before.events)
        return UMI_STATUS_BUSY;
    for (size_t i = 0U; i < review->snapshot.before.events; ++i) {
        status = SameEvent(&review->history[i], &operations->state->events[i], &same);
        if (status != UMI_STATUS_OK) return status;
        if (!same) return UMI_STATUS_BUSY;
    }
#endif
    status = BankHistoryMatches(operations->state, review->snapshot.before.revision,
        review->history, review->snapshot.before.events);
    if (status != UMI_STATUS_OK) return status;
    /* Re-run the canonical transition and atomically validate stored history.
     * A review supplies no alternative commit or balance implementation. */
    return UmiBankOperationsExecute(operations, actor, &review->snapshot.command, outReceipt);
}
