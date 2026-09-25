/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/ledger.c
 *
 * PURPOSE:
 *   Compose canonical balanced journals and derive checked balances and statements.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

/* Each posting has exactly one debit and one equal credit in one currency.
 * Reuse the canonical journal API; Bank supplies orchestration and currency
 * context, not an alternative double-entry representation. */
UmiStatus BankPost(BankState *state, const UmiBankCommand *command,
    const char *debitAccount, const char *creditAccount, UmiMoney amount, bool reversal)
{
    UmiBankJournal *journal;
    UmiAccountingJournalLine line;
    char id[UMI_FINANCE_ID_CAPACITY];
    UmiStatus status;
    int written;
    if (state->counts.journals >= UMI_BANK_EVENT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (amount.minor_units <= 0 || strcmp(debitAccount, creditAccount) == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    journal = &state->journals[state->counts.journals];
    memset(journal, 0, sizeof *journal);
    written = snprintf(id, sizeof id, "bank-journal-%" PRIu64, state->counts.revision + 1U);
    if (written < 0 || (size_t)written >= sizeof id) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_accounting_journal_entry_init(&journal->entry, id, command->businessDate);
    if (status != UMI_STATUS_OK) return status;
    status = umi_accounting_journal_line_init(&line, "debit", debitAccount, amount.minor_units, 0);
    if (status == UMI_STATUS_OK) status = umi_accounting_journal_entry_add_line(&journal->entry, &line);
    if (status != UMI_STATUS_OK) return status;
    status = umi_accounting_journal_line_init(&line, "credit", creditAccount, 0, amount.minor_units);
    if (status == UMI_STATUS_OK) status = umi_accounting_journal_entry_add_line(&journal->entry, &line);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_accounting_journal_entry_balanced(&journal->entry)) return UMI_STATUS_INTERNAL_ERROR;
    journal->entry.status = UMI_ACCOUNTING_JOURNAL_POSTED;
    journal->currency = amount.currency;
    journal->scale = amount.scale;
    journal->revision = state->counts.revision + 1U;
    journal->referenceId = command->id;
    journal->reversal = reversal;
    ++state->counts.journals;
    return UMI_STATUS_OK;
}

/* Incremental checked arithmetic avoids overflow even for INT64_MAX balances.
 * The journal has one applicable line per customer account and operation. */
static UmiStatus ApplyLine(int64_t *balance, const UmiAccountingJournalLine *line)
{
    if (line->debit_minor < 0 || line->credit_minor < 0 ||
        *balance > INT64_MAX - line->credit_minor || *balance < line->debit_minor)
        return UMI_STATUS_INVALID_STATE;
    *balance += line->credit_minor;
    *balance -= line->debit_minor;
    return UMI_STATUS_OK;
}

static UmiStatus Reserve(int64_t *reserved, int64_t amount)
{
    if (amount < 0 || *reserved > INT64_MAX - amount) return UMI_STATUS_CAPACITY_EXCEEDED;
    *reserved += amount;
    return UMI_STATUS_OK;
}

UmiStatus BankProjectBalance(const BankState *state, const char *accountId, UmiBankBalance *out)
{
    UmiBankBalance result = {0};
    int index = BankFindAccount(state, accountId);
    UmiStatus status;
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (index < 0) return UMI_STATUS_NOT_FOUND;
    result.booked.currency = state->accounts[index].account.currency;
    result.booked.scale = state->accounts[index].scale;
    result.reserved = result.available = result.booked;
    result.revision = state->counts.revision;
    for (size_t i = 0U; i < state->counts.journals; ++i) {
        const UmiBankJournal *journal = &state->journals[i];
        for (size_t j = 0U; j < journal->entry.line_count; ++j) {
            const UmiAccountingJournalLine *line = &journal->entry.lines[j];
            if (strcmp(line->account_id.value, accountId) != 0) continue;
            if (journal->scale != result.booked.scale ||
                !umi_accounting_currency_equal(journal->currency, result.booked.currency))
                return UMI_STATUS_INVALID_STATE;
            status = ApplyLine(&result.booked.minor_units, line);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    for (size_t i = 0U; i < state->counts.holds; ++i) {
        const UmiBankHold *hold = &state->holds[i];
        if (hold->state == UMI_BANK_HOLD_ACTIVE && strcmp(hold->accountId.value, accountId) == 0) {
            status = Reserve(&result.reserved.minor_units, hold->amount.minor_units);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    for (size_t i = 0U; i < state->counts.transfers; ++i) {
        const UmiBankTransfer *transfer = &state->transfers[i];
        if ((transfer->state == UMI_BANK_TRANSFER_PENDING || transfer->state == UMI_BANK_TRANSFER_APPROVED) &&
            strcmp(transfer->sourceAccountId.value, accountId) == 0) {
            status = Reserve(&result.reserved.minor_units, transfer->amount.minor_units);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    if (result.reserved.minor_units > result.booked.minor_units) return UMI_STATUS_INVALID_STATE;
    result.available.minor_units = result.booked.minor_units - result.reserved.minor_units;
    *out = result;
    return UMI_STATUS_OK;
}

UmiStatus UmiBankOperationsStatement(const UmiBankOperations *operations,
    const char *accountId, uint64_t firstRevision, uint64_t lastRevision,
    UmiBankStatement *outStatement)
{
    const BankState *state;
    int index;
    int64_t balance = 0;
    UmiStatus status;
    if (outStatement != NULL) memset(outStatement, 0, sizeof *outStatement);
    if (operations == NULL || outStatement == NULL || accountId == NULL ||
        firstRevision == 0U || firstRevision > lastRevision)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (operations->poisoned || operations->state == NULL) return UMI_STATUS_INVALID_STATE;
    state = operations->state;
    if (lastRevision > state->counts.revision) return UMI_STATUS_INVALID_ARGUMENT;
    index = BankFindAccount(state, accountId);
    if (index < 0) return UMI_STATUS_NOT_FOUND;
    outStatement->accountId = state->accounts[index].account.account_id;
    outStatement->opening.currency = state->accounts[index].account.currency;
    outStatement->opening.scale = state->accounts[index].scale;
    outStatement->closing = outStatement->opening;
    outStatement->firstRevision = firstRevision;
    outStatement->lastRevision = lastRevision;
    for (size_t i = 0U; i < state->counts.journals; ++i) {
        const UmiBankJournal *journal = &state->journals[i];
        if (journal->revision > lastRevision) break;
        for (size_t j = 0U; j < journal->entry.line_count; ++j) {
            const UmiAccountingJournalLine *line = &journal->entry.lines[j];
            UmiBankStatementLine *row;
            if (strcmp(line->account_id.value, accountId) != 0) continue;
            status = ApplyLine(&balance, line);
            if (status != UMI_STATUS_OK) { memset(outStatement, 0, sizeof *outStatement); return status; }
            if (journal->revision < firstRevision) { outStatement->opening.minor_units = balance; continue; }
            if (outStatement->count >= UMI_BANK_STATEMENT_CAPACITY) {
                memset(outStatement, 0, sizeof *outStatement); return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            row = &outStatement->lines[outStatement->count++];
            row->revision = journal->revision;
            row->journalId = journal->entry.id;
            row->referenceId = journal->referenceId;
            row->businessDate = journal->entry.accounting_date;
            row->debitMinor = line->debit_minor;
            row->creditMinor = line->credit_minor;
            row->balanceMinor = balance;
        }
    }
    outStatement->closing.minor_units = balance;
    return UMI_STATUS_OK;
}
