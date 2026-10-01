/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_operations/review.h
 * PURPOSE: Review the existing banking transition without publishing its candidate.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_OPERATIONS_REVIEW_H
#define UMICOM_BANK_OPERATIONS_REVIEW_H
#include "umicom/bank_operations/operations.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_BANK_REVIEW_TEXT_CAPACITY 65536U
/** Owned immutable review. Independent of the originating service lifetime.
 * This is a local-simulation preview, not authentication, approval, a signature,
 * a reservation or a guarantee of execution. Use the service's serial queue.
 * The review binds canonical event history, exact command, actor and capability
 * flags. Equivalent history may be reviewed/applied through another handle.
 * It is not tied to a filesystem name. Reload before reviewing external work. */
typedef struct UmiBankReview UmiBankReview;
typedef struct UmiBankReviewAccount {
    UmiFinancialId accountId;
    bool existedBefore;
    bool existedAfter;
    bool activeBefore;
    bool activeAfter;
    UmiBankBalance before;
    UmiBankBalance after;
} UmiBankReviewAccount;
/** All outputs are copies. after.revision is predicted, never a receipt.
 * Account rows contain changed balances or changed debit eligibility, including
 * the effect of a customer being blocked. The command and counts describe
 * actions without monetary effects. A journal/reconciliation is a candidate. */
typedef struct UmiBankReviewSnapshot {
    UmiBankActor actor;
    UmiBankCommand command;
    UmiBankCounts before;
    UmiBankCounts after;
    bool alreadyRecorded;
    uint64_t receiptRevision;
    size_t accountCount;
    UmiBankReviewAccount accounts[UMI_BANK_RECORD_CAPACITY];
    bool hasTransfer;
    bool transferExistedBefore;
    UmiBankTransfer transferBefore;
    UmiBankTransfer transferAfter;
    bool hasJournal;
    UmiBankJournal journal;
    bool hasReconciliation;
    UmiBankReconciliation reconciliation;
    bool hasInterest;
    bool interestExistedBefore;
    UmiBankInterestRequest interestBefore;
    UmiBankInterestRequest interestAfter;
    bool hasCharge;
    bool chargeExistedBefore;
    UmiBankChargeRequest chargeBefore;
    UmiBankChargeRequest chargeAfter;
    /* A resolution edits disposition, not the original comparison. Copy both
     * states and the selected evidence so the review is self-contained. */
    bool reconciliationExistedBefore;
    UmiBankReconciliation reconciliationBefore;
    bool hasReconciliationEvidence;
    UmiBankReconciliation reconciliationEvidence;
    /* Appended resolved hold evidence. Rebuild all consumers together. The
     * authorised amount differs from capturedMinor after a partial final capture. */
    bool hasHold;
    bool holdExistedBefore;
    UmiBankHold holdBefore;
    UmiBankHold holdAfter;
} UmiBankReviewSnapshot;
/** No Data Server write or reload. The exact existing domain transition runs
 * against a disposable copy of the cached state. Failure sets *outReview=NULL. */
UmiStatus UmiBankOperationsReview(const UmiBankOperations *operations,
    const UmiBankActor *actor, const UmiBankCommand *command,
    UmiBankReview **outReview);
void UmiBankReviewDestroy(UmiBankReview *review);
UmiStatus UmiBankReviewSnapshotRead(const UmiBankReview *review,
    UmiBankReviewSnapshot *outSnapshot);
/** Check the complete form, including its revision and current actor flags.
 * Invalid input is reported separately; *outMatches is false on failure. */
UmiStatus UmiBankReviewMatches(const UmiBankReview *review,
    const UmiBankActor *actor, const UmiBankCommand *command, bool *outMatches);
/** Apply only the stored command. Current actor must still match, all local
 * history must agree, and repository validation/commit must succeed. On any
 * failure receipt is zero. Never auto-retry BUSY: reload, review and decide.
 * Reapplying after a successful new commit is BUSY; create a fresh review to
 * inspect the historical idempotent receipt. No database or network rollback
 * is implied by destroying a review. */
UmiStatus UmiBankOperationsExecuteReviewed(UmiBankOperations *operations,
    const UmiBankActor *currentActor, const UmiBankReview *review,
    UmiBankReceipt *outReceipt);
/** Complete bounded plain-text description. No files, markup or locale changes.
 * required size includes NUL; insufficient capacity returns CAPACITY_EXCEEDED
 * and clears output[0] when supplied. NULL/0 can be used to measure. */
UmiStatus UmiBankReviewDescribe(const UmiBankReview *review, char *output,
    size_t capacity, size_t *outRequired);
#ifdef __cplusplus
}
#endif
#endif
