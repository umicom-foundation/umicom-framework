/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/activity.c
 * PURPOSE: Project copied canonical statement entries with checked totals and retained posting evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "activity_private.h"
#include "internal.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
static int ActivityMatches(const UmiBankActivityQuery *query, const UmiBankStatementLine *line)
{
    if (query->fromDate.year != 0 && umi_financial_date_compare(line->businessDate, query->fromDate) < 0) return 0;
    if (query->toDate.year != 0 && umi_financial_date_compare(line->businessDate, query->toDate) > 0) return 0;
    if (query->direction == UMI_BANK_ACTIVITY_DEBITS && line->debitMinor == 0) return 0;
    if (query->direction == UMI_BANK_ACTIVITY_CREDITS && line->creditMinor == 0) return 0;
    return query->reference[0] == '\0' || strstr(line->referenceId.value, query->reference) != NULL ||
        strstr(line->journalId.value, query->reference) != NULL;
}
UmiStatus UmiBankActivityCapture(const UmiBankOperations *operations,
    const UmiBankActivityQuery *query, UmiBankActivity **outActivity)
{
    if (outActivity == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outActivity = NULL;
    UmiStatus status = UmiBankActivityQueryValidate(query);
    if (status != UMI_STATUS_OK) return status;
    UmiBankCounts counts;
    status = UmiBankOperationsCounts(operations, &counts);
    if (status != UMI_STATUS_OK) return status;
    if (counts.events > UMI_BANK_EVENT_CAPACITY || counts.journals > UMI_BANK_EVENT_CAPACITY ||
        counts.revision != counts.events) return UMI_STATUS_INVALID_STATE;
    /* Balance supplies the complete context and rejects unknown accounts
     * before an empty event stream can be mistaken for an empty report. */
    UmiBankBalance balance;
    status = UmiBankOperationsBalance(operations, query->accountId.value, &balance);
    if (status != UMI_STATUS_OK) return status;
    if (counts.revision == 0 || balance.revision != counts.revision) return UMI_STATUS_INVALID_STATE;
    UmiBankStatement *statement = calloc(1, sizeof *statement);
    UmiBankActivity *activity = calloc(1, sizeof *activity);
    if (statement == NULL || activity == NULL) { free(statement); free(activity); return UMI_STATUS_OUT_OF_MEMORY; }
    /* All balance arithmetic and posting order come from the canonical ledger.
     * Filtering never invents an alternative opening or running balance. */
    status = UmiBankOperationsStatement(operations, query->accountId.value, 1, counts.revision, statement);
    if (status == UMI_STATUS_OK && (statement->count > UMI_BANK_STATEMENT_CAPACITY ||
        statement->closing.minor_units != balance.booked.minor_units)) status = UMI_STATUS_INVALID_STATE;
    activity->summary.query = *query;
    activity->summary.balance = balance;
    activity->summary.revision = counts.revision;
    activity->summary.durable = counts.durable;
    activity->summary.totalPostings = statement->count;
    for (size_t i = 0; status == UMI_STATUS_OK && i < statement->count; ++i) {
        const UmiBankStatementLine *line = &statement->lines[i];
        if (!umi_financial_date_is_valid(line->businessDate) || line->revision == 0 ||
            line->revision > counts.revision || line->debitMinor < 0 || line->creditMinor < 0 ||
            (i != 0 && line->revision <= statement->lines[i-1].revision)) { status = UMI_STATUS_INVALID_STATE; break; }
        if (!ActivityMatches(query, line)) continue;
        const UmiBankAuditEvent *event = &operations->state->events[(size_t)(line->revision - 1U)];
        const UmiBankJournal *journal = NULL;
        for (size_t j = 0; j < counts.journals; ++j) {
            if (strcmp(operations->state->journals[j].entry.id.value, line->journalId.value) == 0) {
                journal = &operations->state->journals[j]; break;
            }
        }
        if (journal == NULL || journal->revision != line->revision || event->revision != line->revision ||
            strcmp(event->command.id.value, line->referenceId.value) != 0 ||
            umi_financial_date_compare(event->command.businessDate, line->businessDate) != 0 ||
            UmiBankActionName(event->command.action) == NULL) { status = UMI_STATUS_INVALID_STATE; break; }
        if (activity->summary.debitMinor > INT64_MAX - line->debitMinor ||
            activity->summary.creditMinor > INT64_MAX - line->creditMinor) { status = UMI_STATUS_CAPACITY_EXCEEDED; break; }
        UmiBankActivityRow *row = &activity->rows[activity->summary.count++];
        row->posting = *line; row->actorId = event->actor.id; row->action = event->command.action; row->reversal = journal->reversal;
        activity->summary.debitMinor += line->debitMinor;
        activity->summary.creditMinor += line->creditMinor;
    }
    free(statement);
    if (status != UMI_STATUS_OK) { free(activity); return status; }
    activity->summary.netMinor = activity->summary.creditMinor - activity->summary.debitMinor;
    *outActivity = activity;
    return UMI_STATUS_OK;
}
void UmiBankActivityDestroy(UmiBankActivity *activity) { free(activity); }
UmiStatus UmiBankActivityReadSummary(const UmiBankActivity *activity, UmiBankActivitySummary *out)
{
    if (activity == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = activity->summary; return UMI_STATUS_OK;
}
UmiStatus UmiBankActivityRowAt(const UmiBankActivity *activity, size_t index, UmiBankActivityRow *out)
{
    if (activity == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= activity->summary.count) return UMI_STATUS_NOT_FOUND;
    *out = activity->rows[index]; return UMI_STATUS_OK;
}
