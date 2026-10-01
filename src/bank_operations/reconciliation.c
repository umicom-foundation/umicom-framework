/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/reconciliation.c
 * PURPOSE: Resolve investigated comparisons without manufacturing matching ledger entries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/bank_operations/reconciliation.h"
#include <string.h>

/* Freshness follows committed evidence order, independent of user-entered
 * business dates. A debit and compensating credit still invalidate old proof. */
static bool AccountPostedAfter(const BankState *state, const char *account, uint64_t revision)
{
    for (size_t i = 0U; i < state->counts.journals; ++i) {
        const UmiBankJournal *journal = &state->journals[i];
        if (journal->revision <= revision) continue;
        for (size_t n = 0U; n < journal->entry.line_count; ++n)
            if (strcmp(journal->entry.lines[n].account_id.value, account) == 0) return true;
    }
    return false;
}

/* Replay and reviewed execution use this same transition on disposable state.
 * Original observed balances and their matched result remain evidence forever. */
UmiStatus BankApplyReconciliationReview(BankState *state, const UmiBankActor *actor,
    const UmiBankCommand *command)
{
    int index = BankFindReconciliation(state, command->id.value);
    if (index < 0) return UMI_STATUS_NOT_FOUND;
    UmiBankReconciliation *record = &state->reconciliations[index];
    if (record->matched) return UMI_STATUS_INVALID_STATE;
    if (command->action == UMI_BANK_RECONCILIATION_RESOLVE) {
        if (record->disposition == UMI_BANK_RECONCILIATION_RESOLVED) return UMI_STATUS_INVALID_STATE;
        int evidenceIndex = BankFindReconciliation(state, command->ownerId.value);
        if (evidenceIndex < 0) return UMI_STATUS_NOT_FOUND;
        const UmiBankReconciliation *evidence = &state->reconciliations[evidenceIndex];
        uint64_t after = record->reviewedRevision > record->revision ? record->reviewedRevision : record->revision;
        if (!evidence->matched || strcmp(evidence->accountId.value, record->accountId.value) != 0 ||
            evidence->revision <= after) return UMI_STATUS_INVALID_STATE;
        for (size_t i = 0U; i < state->counts.reconciliations; ++i) {
            const UmiBankReconciliation *later = &state->reconciliations[i];
            if (strcmp(later->accountId.value, record->accountId.value) == 0 && later->revision > evidence->revision)
                return UMI_STATUS_BUSY;
        }
        if (AccountPostedAfter(state, record->accountId.value, evidence->revision)) return UMI_STATUS_BUSY;
        UmiBankBalance balance;
        UmiStatus status = BankProjectBalance(state, record->accountId.value, &balance);
        if (status != UMI_STATUS_OK) return status;
        if (balance.booked.minor_units != evidence->bookedBalance.minor_units ||
            balance.booked.scale != evidence->bookedBalance.scale ||
            !umi_accounting_currency_equal(balance.booked.currency, evidence->bookedBalance.currency))
            return UMI_STATUS_BUSY;
        record->disposition = UMI_BANK_RECONCILIATION_RESOLVED;
        record->evidenceId = evidence->id;
    } else if (command->action == UMI_BANK_RECONCILIATION_REOPEN) {
        if (record->disposition != UMI_BANK_RECONCILIATION_RESOLVED) return UMI_STATUS_INVALID_STATE;
        record->disposition = UMI_BANK_RECONCILIATION_REOPENED;
    } else return UMI_STATUS_NOT_IMPLEMENTED;
    record->reviewedBy = actor->id;
    memcpy(record->reviewReason, command->name, sizeof record->reviewReason);
    record->reviewedRevision = state->counts.revision + 1U;
    return UMI_STATUS_OK;
}

/* Copy by stable identity, preserving the service's existing query ownership. */
UmiStatus UmiBankOperationsFindReconciliation(const UmiBankOperations *operations,
    const char *id, UmiBankReconciliation *outRecord)
{
    if (outRecord != NULL) memset(outRecord, 0, sizeof *outRecord);
    if (operations == NULL || id == NULL || outRecord == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiFinancialId key = {0}; size_t length = 0U;
    while (length < sizeof key.value && id[length] != '\0') ++length;
    if (length == sizeof key.value) return UMI_STATUS_INVALID_ARGUMENT;
    memcpy(key.value, id, length + 1U);
    if (!BankIdValid(&key, true)) return UMI_STATUS_INVALID_ARGUMENT;
    if (operations->poisoned || operations->state == NULL) return UMI_STATUS_INVALID_STATE;
    int index = BankFindReconciliation(operations->state, key.value);
    if (index < 0) return UMI_STATUS_NOT_FOUND;
    *outRecord = operations->state->reconciliations[index]; return UMI_STATUS_OK;
}

/* A matched original comparison never masquerades as a resolved mismatch. */
const char *UmiBankReconciliationStateName(const UmiBankReconciliation *record)
{
    if (record == NULL || record->disposition < UMI_BANK_RECONCILIATION_UNREVIEWED ||
        record->disposition > UMI_BANK_RECONCILIATION_REOPENED) return "Unknown";
    if (record->matched) return "Matched";
    if (record->disposition == UMI_BANK_RECONCILIATION_RESOLVED) return "Resolved break";
    if (record->disposition == UMI_BANK_RECONCILIATION_REOPENED) return "Reopened break";
    return "Open break";
}
