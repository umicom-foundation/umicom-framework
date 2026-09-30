/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance_operations/close_review.c
 * PURPOSE:
 *   Shared close predicate and owned explanations. No storage or network I/O.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Shared close predicate and owned explanations. No storage or network I/O.
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include "umicom/finance_operations/close_review.h"
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLOSE_ISSUES (UMI_FINANCE_OPERATIONS_PERIODS + UMI_FINANCE_OPERATIONS_JOURNALS + \
    UMI_FINANCE_OPERATIONS_ORDERS + UMI_FINANCE_OPERATIONS_FILLS + 2U * UMI_FINANCE_OPERATIONS_ACCOUNTS)
struct UmiFinanceCloseReview {
    UmiFinanceCloseReviewInfo info;
    UmiFinanceCloseIssue issues[CLOSE_ISSUES];
    UmiFinanceTrialBalance balances[UMI_FINANCE_OPERATIONS_ACCOUNTS];
};
static bool Bounds(const FinanceState *s)
{
    if (!s || s->counts.periods > UMI_FINANCE_OPERATIONS_PERIODS ||
        s->counts.journals > UMI_FINANCE_OPERATIONS_JOURNALS ||
        s->counts.orders > UMI_FINANCE_OPERATIONS_ORDERS ||
        s->counts.fills > UMI_FINANCE_OPERATIONS_FILLS ||
        s->counts.accounts > UMI_FINANCE_OPERATIONS_ACCOUNTS ||
        s->counts.reconciliations > UMI_FINANCE_OPERATIONS_RECONCILIATIONS)
        return false;
    for (size_t i = 0; i < s->counts.journals; ++i)
        if (s->journals[i].entry.line_count > UMI_FINANCE_OPERATIONS_JOURNAL_LINES)
            return false;
    return true;
}
static UmiStatus AddIssue(UmiFinanceCloseReview *r, UmiFinanceCloseIssueKind kind,
    UmiFinancialId id, const UmiFinanceOperationReconciliation *evidence,
    uint64_t postingRevision, UmiStatus status)
{
    if (!r) return status;
    if (r->info.issueCount >= CLOSE_ISSUES) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiFinanceCloseIssue *issue = &r->issues[r->info.issueCount++];
    issue->kind = kind; issue->entityId = id; issue->status = status;
    issue->requiredPostingRevision = postingRevision;
    if (evidence) {
        issue->evidenceId = evidence->id;
        issue->observedPostingRevision = evidence->postingRevision;
    }
    return status;
}
/* Preserve the original fail-fast order for commands. A snapshot records all
 * blockers in that order and the same first failure, so inspection and Apply
 * cannot disagree merely because the UI has a separate implementation. */
UmiStatus FinanceEvaluateClose(const FinanceState *s, size_t p, UmiFinanceCloseReview *r)
{
    UmiStatus first = UMI_STATUS_OK;
    if (!Bounds(s)) return UMI_STATUS_PARSE_ERROR;
    if (p >= s->counts.periods) return UMI_STATUS_NOT_FOUND;
    const UmiFinanceOperationPeriod *period = &s->periods[p];
#define BLOCK(kind, id, evidence, posting, result) do { \
    UmiStatus failure = AddIssue(r, kind, id, evidence, posting, result); \
    if (!r) return failure; \
    if (first == UMI_STATUS_OK) first = failure; \
} while (0)
    for (size_t i = 0; i < s->counts.periods; ++i)
        if (i != p && umi_financial_date_compare(s->periods[i].endDate, period->startDate) < 0 &&
            s->periods[i].status != UMI_ACCOUNTING_PERIOD_CLOSED)
            BLOCK(UMI_FINANCE_CLOSE_EARLIER_PERIOD, s->periods[i].id, NULL, 0U, UMI_STATUS_INVALID_STATE);
    for (size_t i = 0; i < s->counts.journals; ++i) {
        const UmiFinanceOperationJournal *j = &s->journals[i];
        if (FinanceIdEqual(j->periodId, period->id) && j->entry.status != UMI_ACCOUNTING_JOURNAL_POSTED &&
            j->entry.status != UMI_ACCOUNTING_JOURNAL_REVERSED)
            BLOCK(UMI_FINANCE_CLOSE_UNPOSTED_JOURNAL, j->entry.id, NULL, 0U, UMI_STATUS_INVALID_STATE);
    }
    for (size_t i = 0; i < s->counts.orders; ++i)
        if (s->orders[i].remainingLots > 0 &&
            umi_financial_date_compare(s->orders[i].date, period->endDate) <= 0)
            BLOCK(UMI_FINANCE_CLOSE_OPEN_ORDER, s->orders[i].id, NULL, 0U, UMI_STATUS_INVALID_STATE);
    for (size_t i = 0; i < s->counts.fills; ++i)
        if (s->fills[i].state != UMI_SETTLEMENT_SETTLED &&
            umi_financial_date_compare(s->fills[i].tradeDate, period->endDate) <= 0)
            BLOCK(UMI_FINANCE_CLOSE_UNSETTLED_FILL, s->fills[i].id, NULL, 0U, UMI_STATUS_INVALID_STATE);
    for (size_t i = 0; i < s->counts.accounts; ++i) {
        const UmiFinanceOperationAccount *a = &s->accounts[i];
        const UmiFinanceOperationReconciliation *latest = NULL;
        if (a->lastPostingRevision != 0U) {
            for (size_t j = 0; j < s->counts.reconciliations; ++j)
                if (FinanceIdEqual(s->reconciliations[j].accountId, a->id)) latest = &s->reconciliations[j];
            if (!latest)
                BLOCK(UMI_FINANCE_CLOSE_MISSING_RECONCILIATION, a->id, NULL, a->lastPostingRevision, UMI_STATUS_INVALID_STATE);
            else if (!latest->matched)
                BLOCK(UMI_FINANCE_CLOSE_MISMATCHED_RECONCILIATION, a->id, latest, a->lastPostingRevision, UMI_STATUS_INVALID_STATE);
            else if (latest->postingRevision != a->lastPostingRevision)
                BLOCK(UMI_FINANCE_CLOSE_STALE_RECONCILIATION, a->id, latest, a->lastPostingRevision, UMI_STATUS_INVALID_STATE);
        }
        UmiFinanceTrialBalance b;
        UmiStatus status = FinanceTrialBalance(s, period->id, a->currency, a->scale, &b);
        if (status != UMI_STATUS_OK) {
            BLOCK(UMI_FINANCE_CLOSE_TRIAL_BALANCE_UNAVAILABLE, a->id, NULL, 0U, status);
        } else {
            if (!b.balanced)
                BLOCK(UMI_FINANCE_CLOSE_UNBALANCED_TRIAL_BALANCE, a->id, NULL, 0U, UMI_STATUS_INVALID_STATE);
            if (r) {
                size_t n = 0;
                while (n < r->info.currencyCount &&
                    (!umi_accounting_currency_equal(r->balances[n].currency, b.currency) ||
                     r->balances[n].scale != b.scale)) ++n;
                if (n == r->info.currencyCount) {
                    if (n >= UMI_FINANCE_OPERATIONS_ACCOUNTS) return UMI_STATUS_CAPACITY_EXCEEDED;
                    r->balances[n] = b; ++r->info.currencyCount;
                }
            }
        }
    }
#undef BLOCK
    return first;
}
UmiStatus UmiFinanceCloseReviewCreate(const UmiFinanceOperations *o,
    const char *periodId, UmiFinanceCloseReview **out)
{
    if (!out) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (!o || !periodId) return UMI_STATUS_INVALID_ARGUMENT;
    if (o->poisoned || !o->state) return UMI_STATUS_INVALID_STATE;
    if (!Bounds(o->state)) return UMI_STATUS_PARSE_ERROR;
    UmiFinancialId id;
    UmiStatus status = FinanceSetId(&id, periodId);
    if (status != UMI_STATUS_OK) return status;
    size_t p = FinancePeriodIndex(o->state, id);
    if (p == FINANCE_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    UmiFinanceCloseReview *r = calloc(1U, sizeof *r);
    if (!r) return UMI_STATUS_OUT_OF_MEMORY;
    r->info.period = o->state->periods[p];
    r->info.revision = o->state->counts.revision;
    r->info.writesBlocked = o->writesBlocked;
    r->info.durable = o->state->counts.durable;
    status = FinanceEvaluateClose(o->state, p, r);
    /* Non-business errors are retained as issues when an account explains them.
     * A failure without an issue means construction did not complete. */
    if (status != UMI_STATUS_OK && r->info.issueCount == 0U) { free(r); return status; }
    r->info.evaluationStatus = status;
    r->info.checksPass = status == UMI_STATUS_OK;
    r->info.canPrepare = r->info.checksPass && !o->writesBlocked && r->info.period.status == UMI_ACCOUNTING_PERIOD_OPEN;
    r->info.canFinalise = r->info.checksPass && !o->writesBlocked && r->info.period.status == UMI_ACCOUNTING_PERIOD_SOFT_CLOSED;
    *out = r;
    return UMI_STATUS_OK;
}
void UmiFinanceCloseReviewDestroy(UmiFinanceCloseReview *r) { free(r); }
UmiStatus UmiFinanceCloseReviewGetInfo(const UmiFinanceCloseReview *r, UmiFinanceCloseReviewInfo *out)
{
    if (!r || !out) return UMI_STATUS_INVALID_ARGUMENT;
    *out = r->info; return UMI_STATUS_OK;
}
UmiStatus UmiFinanceCloseReviewIssueAt(const UmiFinanceCloseReview *r, size_t index, UmiFinanceCloseIssue *out)
{
    if (!r || !out) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= r->info.issueCount) return UMI_STATUS_NOT_FOUND;
    *out = r->issues[index]; return UMI_STATUS_OK;
}
UmiStatus UmiFinanceCloseReviewTrialBalanceAt(const UmiFinanceCloseReview *r, size_t index, UmiFinanceTrialBalance *out)
{
    if (!r || !out) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= r->info.currencyCount) return UMI_STATUS_NOT_FOUND;
    *out = r->balances[index]; return UMI_STATUS_OK;
}
const char *UmiFinanceCloseIssueText(UmiFinanceCloseIssueKind kind)
{
    switch (kind) {
    case UMI_FINANCE_CLOSE_EARLIER_PERIOD: return "Earlier period is not closed";
    case UMI_FINANCE_CLOSE_UNPOSTED_JOURNAL: return "Journal is not posted";
    case UMI_FINANCE_CLOSE_OPEN_ORDER: return "Order still has an open remainder";
    case UMI_FINANCE_CLOSE_UNSETTLED_FILL: return "Fill is not settled locally";
    case UMI_FINANCE_CLOSE_MISSING_RECONCILIATION: return "Posted account has no reconciliation";
    case UMI_FINANCE_CLOSE_MISMATCHED_RECONCILIATION: return "Latest reconciliation records a difference";
    case UMI_FINANCE_CLOSE_STALE_RECONCILIATION: return "Latest reconciliation predates the last posting";
    case UMI_FINANCE_CLOSE_TRIAL_BALANCE_UNAVAILABLE: return "Trial balance could not be calculated";
    case UMI_FINANCE_CLOSE_UNBALANCED_TRIAL_BALANCE: return "Trial balance does not agree";
    default: return "Unknown close issue";
    }
}

typedef struct CloseWriter { char *buffer; size_t capacity, length; bool failed; } CloseWriter;
static void Append(CloseWriter *w, const char *format, ...)
{
    if (w->failed) return;
    va_list args; va_start(args, format);
    int n = vsnprintf(w->buffer ? w->buffer + w->length : NULL,
        w->buffer ? w->capacity - w->length : 0U, format, args);
    va_end(args);
    if (n < 0 || (size_t)n > SIZE_MAX - w->length - 1U ||
        (w->buffer && (size_t)n >= w->capacity - w->length)) { w->failed = true; return; }
    w->length += (size_t)n;
}
static void Format(const UmiFinanceCloseReview *r, CloseWriter *w)
{
    const UmiFinanceCloseReviewInfo *i = &r->info;
    Append(w, "CLOSE REVIEW - %s\nLoaded book revision: %" PRIu64 "\n", i->period.id.value, i->revision);
    Append(w, "Period state: %s\nChecks: %s; blocking records: %zu\n",
        i->period.status == UMI_ACCOUNTING_PERIOD_OPEN ? "open" :
        i->period.status == UMI_ACCOUNTING_PERIOD_SOFT_CLOSED ? "awaiting separate closer" : "closed",
        i->checksPass ? "satisfied in this snapshot" : "not satisfied", i->issueCount);
    Append(w, "Prepare eligible: %s; finalise eligible: %s\n",
        i->canPrepare ? "yes" : "no", i->canFinalise ? "yes" : "no");
    if (i->writesBlocked) Append(w, "Writes are blocked after a failed reload. This is the last loaded view.\n");
    Append(w, "This is not approval, a lock or proof of current external state. Apply rechecks controls.\n");
    if (i->canFinalise) Append(w, "The closing actor must differ from preparer %s.\n", i->period.preparedBy.value);
    for (size_t n = 0; n < i->issueCount; ++n) {
        const UmiFinanceCloseIssue *x = &r->issues[n];
        Append(w, "- %s: %s", x->entityId.value, UmiFinanceCloseIssueText(x->kind));
        if (x->evidenceId.value[0]) Append(w, "; evidence %s", x->evidenceId.value);
        if (x->requiredPostingRevision) Append(w, "; posting %" PRIu64 ", evidence covers %" PRIu64,
            x->requiredPostingRevision, x->observedPostingRevision);
        Append(w, " (status %d)\n", (int)x->status);
    }
    for (size_t n = 0; n < i->currencyCount; ++n) {
        const UmiFinanceTrialBalance *b = &r->balances[n];
        Append(w, "%s scale %u: period debit %" PRId64 ", credit %" PRId64
            "; closing debit %" PRId64 ", credit %" PRId64 " minor units; %s\n",
            b->currency.code, (unsigned)b->scale, b->periodDebitMinor, b->periodCreditMinor,
            b->closingDebitMinor, b->closingCreditMinor, b->balanced ? "balanced" : "NOT balanced");
    }
    Append(w, "A balanced ledger alone does not mean all orders, fills and reconciliation are complete.\n");
}
UmiStatus UmiFinanceCloseReviewFormat(const UmiFinanceCloseReview *r,
    char *buffer, size_t capacity, size_t *required)
{
    if (required) *required = 0U;
    if (buffer && capacity) buffer[0] = '\0';
    if (!r || (!buffer && capacity)) return UMI_STATUS_INVALID_ARGUMENT;
    CloseWriter measure = {NULL, 0U, 0U, false};
    Format(r, &measure);
    if (measure.failed) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (required) *required = measure.length + 1U;
    if (!buffer || capacity <= measure.length) return UMI_STATUS_CAPACITY_EXCEEDED;
    CloseWriter writer = {buffer, capacity, 0U, false};
    Format(r, &writer);
    if (writer.failed) { buffer[0] = '\0'; return UMI_STATUS_INTERNAL_ERROR; }
    return UMI_STATUS_OK;
}
