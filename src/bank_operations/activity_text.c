/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/activity_text.c
 * PURPOSE: Describe filtered totals separately from full balances and original posting balances.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "activity_private.h"
#include "umicom/finance/money_text.h"
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
typedef struct ActivityText { char *text; size_t capacity, length; UmiStatus status; } ActivityText;
static void ActivityAppend(ActivityText *sink, const char *format, ...)
{
    if (sink->status != UMI_STATUS_OK) return;
    size_t remaining = sink->length < sink->capacity ? sink->capacity - sink->length : 0;
    va_list args; va_start(args, format);
    int written = vsnprintf(remaining ? sink->text + sink->length : NULL, remaining, format, args);
    va_end(args);
    if (written < 0 || (size_t)written > SIZE_MAX - sink->length - 1U) sink->status = UMI_STATUS_CAPACITY_EXCEEDED;
    else sink->length += (size_t)written;
}
static void ActivityMoney(ActivityText *sink, UmiMoney value)
{
    char text[UMI_MONEY_TEXT_CAPACITY];
    UmiStatus status = UmiMoneyTextFormat(&value, text, sizeof text, NULL);
    if (status != UMI_STATUS_OK) sink->status = status;
    else ActivityAppend(sink, "%s", text);
}
UmiStatus UmiBankActivityDescribe(const UmiBankActivity *activity,
    char *output, size_t capacity, size_t *outRequired)
{
    if (outRequired != NULL) *outRequired = 0;
    if (output != NULL && capacity != 0) output[0] = '\0';
    if (activity == NULL || (output == NULL && capacity != 0)) return UMI_STATUS_INVALID_ARGUMENT;
    ActivityText sink = {output, capacity, 0, UMI_STATUS_OK};
    const UmiBankActivitySummary *s = &activity->summary;
    char from[11], to[11]; BankActivityDateText(s->query.fromDate, from); BankActivityDateText(s->query.toDate, to);
    ActivityAppend(&sink, "LOCAL PRACTICE ACCOUNT ACTIVITY\nAccount: %s\nCaptured revision: %" PRIu64 "\nStorage: %s\n"
        "Business dates (inclusive): %s to %s\nDirection: %s\nReference/journal contains: %s\n"
        "Matching postings: %zu of %zu\n\nCOMPLETE BALANCES AT CAPTURE (not filtered)\nBooked: ",
        s->query.accountId.value, s->revision, s->durable ? "persistent local SQLite" : "memory only",
        from[0] ? from : "unbounded", to[0] ? to : "unbounded", UmiBankActivityDirectionName(s->query.direction),
        s->query.reference[0] ? s->query.reference : "(any)", s->count, s->totalPostings);
    ActivityMoney(&sink, s->balance.booked); ActivityAppend(&sink, "\nReserved: "); ActivityMoney(&sink, s->balance.reserved);
    ActivityAppend(&sink, "\nAvailable: "); ActivityMoney(&sink, s->balance.available);
    ActivityAppend(&sink, "\n\nMATCHING POSTINGS ONLY\nDebits: ");
    UmiMoney amount = s->balance.booked; amount.minor_units = s->debitMinor; ActivityMoney(&sink, amount);
    ActivityAppend(&sink, "\nCredits: "); amount.minor_units = s->creditMinor; ActivityMoney(&sink, amount);
    ActivityAppend(&sink, "\nNet credits minus debits: "); amount.minor_units = s->netMinor; ActivityMoney(&sink, amount);
    ActivityAppend(&sink, "\nThese totals are not opening, closing or available balances.\n");
    if (s->count == 0) ActivityAppend(&sink, "\nNo postings match these filters. The account still has the complete balances shown above.\n");
    for (size_t i = 0; i < s->count; ++i) {
        const UmiBankActivityRow *row = &activity->rows[i]; const UmiBankStatementLine *line = &row->posting;
        char date[11]; BankActivityDateText(line->businessDate, date);
        ActivityAppend(&sink, "\nPosting revision %" PRIu64 " | business date %s\nJournal: %s\nReference: %s\nActor: %s\nAction: %s%s\nDebit: ",
            line->revision, date, line->journalId.value, line->referenceId.value, row->actorId.value,
            UmiBankActionName(row->action), row->reversal ? " (compensating reversal)" : "");
        amount.minor_units = line->debitMinor; ActivityMoney(&sink, amount);
        ActivityAppend(&sink, "; credit: "); amount.minor_units = line->creditMinor; ActivityMoney(&sink, amount);
        ActivityAppend(&sink, "\nRecorded booked balance after this posting (all prior postings): ");
        amount.minor_units = line->balanceMinor; ActivityMoney(&sink, amount); ActivityAppend(&sink, "\n");
    }
    ActivityAppend(&sink, "\nPosting revision order is retained; backdated business dates do not reorder the ledger.\n"
        "Holds and unposted requests are not journal rows. Reversals remain separate postings.\n"
        "This is captured local simulation evidence. It neither updates automatically nor represents a live bank statement.\n");
    if (sink.status == UMI_STATUS_OK && outRequired != NULL) *outRequired = sink.length + 1U;
    if (sink.status == UMI_STATUS_OK && output != NULL && sink.length >= capacity) sink.status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (sink.status != UMI_STATUS_OK && output != NULL && capacity != 0) output[0] = '\0';
    return sink.status;
}
