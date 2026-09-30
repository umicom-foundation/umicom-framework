/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/review_text.c
 * PURPOSE: Explain a banking candidate without presenting it as a committed result.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/bank_operations/review.h"
#include "umicom/finance/money_text.h"
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ReviewText { char *text; size_t capacity, length; UmiStatus status; } ReviewText;
static void Append(ReviewText *sink, const char *format, ...)
{
    va_list args;
    int written;
    size_t remaining = sink->length < sink->capacity ? sink->capacity - sink->length : 0U;
    if (sink->status != UMI_STATUS_OK) return;
    va_start(args, format);
    written = vsnprintf(remaining != 0U ? sink->text + sink->length : NULL, remaining, format, args);
    va_end(args);
    if (written < 0 || (size_t)written > SIZE_MAX - sink->length - 1U) {
        sink->status = UMI_STATUS_CAPACITY_EXCEEDED; return;
    }
    sink->length += (size_t)written;
}
static void Amount(ReviewText *sink, const UmiMoney *money)
{
    char text[UMI_MONEY_TEXT_CAPACITY];
    UmiStatus status = UmiMoneyTextFormat(money, text, sizeof text, NULL);
    if (status != UMI_STATUS_OK) { sink->status = status; return; }
    Append(sink, "%s", text);
}
static void Balance(ReviewText *sink, const UmiBankBalance *balance)
{
    Append(sink, "booked "); Amount(sink, &balance->booked);
    Append(sink, "; reserved "); Amount(sink, &balance->reserved);
    Append(sink, "; available "); Amount(sink, &balance->available); Append(sink, "\n");
}
static const char *TransferState(UmiBankTransferState state)
{
    switch (state) {
    case UMI_BANK_TRANSFER_PENDING: return "pending approval";
    case UMI_BANK_TRANSFER_APPROVED: return "approved";
    case UMI_BANK_TRANSFER_REJECTED: return "rejected";
    case UMI_BANK_TRANSFER_CANCELLED: return "cancelled";
    case UMI_BANK_TRANSFER_EXECUTED: return "executed locally";
    case UMI_BANK_TRANSFER_REVERSED: return "reversed locally";
    default: return "invalid";
    }
}
UmiStatus UmiBankReviewDescribe(const UmiBankReview *review, char *output, size_t capacity, size_t *outRequired)
{
    UmiBankReviewSnapshot *snapshot;
    ReviewText sink = {output, capacity, 0U, UMI_STATUS_OK};
    UmiStatus status;
    if (outRequired != NULL) *outRequired = 0U;
    if (output != NULL && capacity != 0U) output[0] = '\0';
    if (review == NULL || (output == NULL && capacity != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    snapshot = malloc(sizeof *snapshot);
    if (snapshot == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiBankReviewSnapshotRead(review, snapshot);
    if (status != UMI_STATUS_OK) { free(snapshot); return status; }
    Append(&sink, "LOCAL BANKING REVIEW - no command has been committed by this review.\n");
    Append(&sink, "Action: %s\nActor: %s; capability flags: %" PRIu32 "\nRequest: %s\nEntity: %s\n",
        UmiBankActionName(snapshot->command.action), snapshot->actor.id.value, snapshot->actor.capabilities,
        snapshot->command.requestId.value, snapshot->command.id.value);
    Append(&sink, "Source revision: %" PRIu64 "; predicted revision: %" PRIu64 "\n",
        snapshot->before.revision, snapshot->after.revision);
    Append(&sink, "Business date: %04" PRId32 "-%02u-%02u; request timestamp: %" PRId64 " ms\n",
        snapshot->command.businessDate.year, (unsigned)snapshot->command.businessDate.month,
        (unsigned)snapshot->command.businessDate.day, snapshot->command.timestampMillis);
    Append(&sink, "Owner / beneficiary / card: %s\nSource: %s\nDestination: %s\nName: %s\nRequested state: %d\n",
        snapshot->command.ownerId.value, snapshot->command.sourceAccountId.value,
        snapshot->command.destinationAccountId.value, snapshot->command.name, (int)snapshot->command.state);
    if (snapshot->command.amount.currency.code[0] != '\0') {
        Append(&sink, "Command amount: "); Amount(&sink, &snapshot->command.amount); Append(&sink, "\n");
    }
    if (snapshot->alreadyRecorded)
        Append(&sink, "Already recorded at revision %" PRIu64 ". Applying returns that receipt, not another posting.\n", snapshot->receiptRevision);
    Append(&sink, "Events: %zu -> %zu; journals: %zu -> %zu; holds: %zu -> %zu; transfers: %zu -> %zu\n",
        snapshot->before.events, snapshot->after.events, snapshot->before.journals, snapshot->after.journals,
        snapshot->before.holds, snapshot->after.holds, snapshot->before.transfers, snapshot->after.transfers);
    if (snapshot->hasTransfer) {
        const UmiBankTransfer *transfer = &snapshot->transferAfter;
        Append(&sink, "\nResolved transfer %s: ", transfer->id.value);
        Amount(&sink, &transfer->amount);
        Append(&sink, "\n  From %s to %s; beneficiary %s\n  State: %s -> %s\n  Maker: %s; checker: %s\n",
            transfer->sourceAccountId.value, transfer->destinationAccountId.value,
            transfer->beneficiaryId.value, snapshot->transferExistedBefore ?
                TransferState(snapshot->transferBefore.state) : "not submitted",
            TransferState(transfer->state), transfer->makerId.value,
            transfer->checkerId.value[0] != '\0' ? transfer->checkerId.value : "not assigned");
    }
    if (snapshot->hasInterest) {
        const UmiBankInterestRequest *request = &snapshot->interestAfter;
        Append(&sink, "\nPractice interest %s; account %s; period %s\n  Fixed principal: ",
            request->id.value, request->accountId.value, request->periodId.value);
        Amount(&sink, &request->principal); Append(&sink, "; calculated interest: ");
        Amount(&sink, &request->amount);
        Append(&sink, "\n  Annual rate: %" PRId32 " basis points; days: %" PRIu32 "; basis: %" PRIu32
            "\n  Fractions truncated toward zero once. No historical daily-balance calculation.\n"
            "  Maker: %s; checker: %s; state: %s -> %s\n",
            request->terms.annualRateBps, request->terms.days, request->terms.dayCountBasis,
            request->makerId.value, request->checkerId.value[0] != '\0' ? request->checkerId.value : "not assigned",
            snapshot->interestExistedBefore ? TransferState(snapshot->interestBefore.state) : "not submitted",
            TransferState(request->state));
    }
    Append(&sink, "Affected account balances / debit eligibility: %zu\n", snapshot->accountCount);
    for (size_t i = 0U; i < snapshot->accountCount; ++i) {
        const UmiBankReviewAccount *row = &snapshot->accounts[i];
        Append(&sink, "\nAccount %s\n  Before: ", row->accountId.value);
        if (row->existedBefore) Balance(&sink, &row->before); else Append(&sink, "not open\n");
        Append(&sink, "  After:  "); Balance(&sink, &row->after);
        Append(&sink, "  Account/customer active: %s -> %s\n", row->activeBefore ? "yes" : "no", row->activeAfter ? "yes" : "no");
    }
    if (snapshot->hasJournal) {
        const UmiBankJournal *j = &snapshot->journal;
        Append(&sink, "\nProposed journal %s; reference %s; reversal %s; %.3s scale %u\n",
            j->entry.id.value, j->referenceId.value, j->reversal ? "yes" : "no", j->currency.code, (unsigned)j->scale);
        for (size_t i = 0U; i < j->entry.line_count; ++i) {
            const UmiAccountingJournalLine *line = &j->entry.lines[i];
            Append(&sink, "  %s: debit %" PRId64 "; credit %" PRId64 " minor units\n",
                line->account_id.value, line->debit_minor, line->credit_minor);
        }
    } else Append(&sink, "\nNo new ledger journal is proposed. Approval alone does not transfer cash.\n");
    if (snapshot->hasReconciliation)
        Append(&sink, "Reconciliation: %s. A mismatch records a break; it does not adjust the balance.\n",
            snapshot->reconciliation.matched ? "matched" : "different balances");
    Append(&sink, "\nThis is a prediction from cached state, not a receipt or a real payment.\n"
        "Changed fields, identity or saved state require another review.\n");
    free(snapshot);
    if (outRequired != NULL && sink.status == UMI_STATUS_OK) *outRequired = sink.length + 1U;
    if (sink.status == UMI_STATUS_OK && sink.length >= capacity) sink.status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (sink.status != UMI_STATUS_OK && output != NULL && capacity != 0U) output[0] = '\0';
    return sink.status;
}
