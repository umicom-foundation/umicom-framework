/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance_operations/accounting.c
 *
 * PURPOSE:
 *   Own ledger account creation, maker/checker journals, compensating entries, reconciliation and controlled period close.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <stdio.h>
#include <string.h>

static UmiStatus CurrencyScaleAvailable(const FinanceState *state, UmiCurrency currency, uint8_t scale)
{
    for (size_t i = 0U; i < state->counts.accounts; ++i)
        if (umi_accounting_currency_equal(state->accounts[i].currency, currency) &&
            state->accounts[i].scale != scale) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < state->counts.instruments; ++i)
        if (umi_accounting_currency_equal(state->instruments[i].currency, currency) &&
            state->instruments[i].scale != scale) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

static UmiStatus CreateAccount(FinanceState *state, const UmiFinanceOperationCommand *command)
{
    UmiFinanceOperationAccount *account;
    UmiStatus status;
    if (!FinanceCurrencyValid(command->currency) || command->name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (FinanceAccountIndex(state, command->id) != FINANCE_INDEX_NONE) return UMI_STATUS_ALREADY_EXISTS;
    status = CurrencyScaleAvailable(state, command->currency, command->scale);
    if (status != UMI_STATUS_OK) return status;
    if (state->counts.accounts >= UMI_FINANCE_OPERATIONS_ACCOUNTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    account = &state->accounts[state->counts.accounts++];
    memset(account, 0, sizeof *account);
    account->id = command->id;
    memcpy(account->name, command->name, sizeof account->name);
    account->currency = command->currency;
    account->scale = command->scale;
    account->accountClass = command->accountClass;
    return UMI_STATUS_OK;
}

static UmiStatus OpenPeriod(FinanceState *state, const UmiFinanceOperationCommand *command)
{
    UmiFinanceOperationPeriod *period;
    if (!umi_financial_date_is_valid(command->date) || !umi_financial_date_is_valid(command->endDate) ||
        umi_financial_date_compare(command->date, command->endDate) > 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (FinancePeriodIndex(state, command->id) != FINANCE_INDEX_NONE) return UMI_STATUS_ALREADY_EXISTS;
    for (size_t i = 0U; i < state->counts.periods; ++i) {
        const UmiFinanceOperationPeriod *existing = &state->periods[i];
        if (umi_financial_date_compare(command->date, existing->endDate) <= 0 &&
            umi_financial_date_compare(command->endDate, existing->startDate) >= 0) return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (state->counts.periods >= UMI_FINANCE_OPERATIONS_PERIODS) return UMI_STATUS_CAPACITY_EXCEEDED;
    period = &state->periods[state->counts.periods++];
    memset(period, 0, sizeof *period);
    period->id = command->id;
    period->startDate = command->date;
    period->endDate = command->endDate;
    period->status = UMI_ACCOUNTING_PERIOD_OPEN;
    return UMI_STATUS_OK;
}

/* Canonical journal primitives remain unchanged. This transactional layer first
 * verifies every line, account, currency and checked total, then uses the shared
 * balanced-entry contract. Unsafe totals are never passed to that older API. */
static UmiStatus ValidateJournal(const FinanceState *state, UmiFinanceOperationJournal *journal)
{
    const size_t periodIndex = FinancePeriodIndex(state, journal->periodId);
    int64_t debit = 0, credit = 0;
    UmiStatus status;
    if (periodIndex == FINANCE_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    if (!FinanceDateInPeriod(journal->entry.accounting_date, &state->periods[periodIndex]) ||
        state->periods[periodIndex].status != UMI_ACCOUNTING_PERIOD_OPEN) return UMI_STATUS_INVALID_STATE;
    if (!umi_accounting_journal_entry_valid(&journal->entry) || journal->entry.line_count < 2U ||
        journal->entry.line_count > UMI_FINANCE_OPERATIONS_JOURNAL_LINES) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < journal->entry.line_count; ++i) {
        const UmiAccountingJournalLine *line = &journal->entry.lines[i];
        const size_t accountIndex = FinanceAccountIndex(state, line->account_id);
        const UmiFinanceOperationAccount *account;
        if (!FinanceIdValid(&line->id, true) || !FinanceIdValid(&line->account_id, true) ||
            !umi_accounting_journal_line_valid(line)) return UMI_STATUS_INVALID_ARGUMENT;
        if (accountIndex == FINANCE_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
        account = &state->accounts[accountIndex];
        if (i == 0U) { journal->currency = account->currency; journal->scale = account->scale; }
        else if (!umi_accounting_currency_equal(journal->currency, account->currency) ||
            journal->scale != account->scale) return UMI_STATUS_INVALID_ARGUMENT;
        status = FinanceAdd(debit, line->debit_minor, &debit);
        if (status == UMI_STATUS_OK) status = FinanceAdd(credit, line->credit_minor, &credit);
        if (status != UMI_STATUS_OK) return status;
    }
    if (debit == 0 || debit != credit || !umi_accounting_journal_entry_balanced(&journal->entry))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

static UmiStatus EnterJournal(FinanceState *state, const UmiFinanceOperationCommand *command)
{
    UmiFinanceOperationJournal journal;
    UmiStatus status;
    if (!FinanceIdValid(&command->referenceId, true) || command->lineCount < 2U) return UMI_STATUS_INVALID_ARGUMENT;
    if (FinanceJournalIndex(state, command->id) != FINANCE_INDEX_NONE) return UMI_STATUS_ALREADY_EXISTS;
    if (state->counts.journals >= UMI_FINANCE_OPERATIONS_JOURNALS) return UMI_STATUS_CAPACITY_EXCEEDED;
    memset(&journal, 0, sizeof journal);
    status = umi_accounting_journal_entry_init(&journal.entry, command->id.value, command->date);
    journal.periodId = command->referenceId;
    journal.makerId = command->actorId;
    for (size_t i = 0U; i < command->lineCount && status == UMI_STATUS_OK; ++i) {
        UmiAccountingJournalLine line;
        char lineId[24];
        int written = snprintf(lineId, sizeof lineId, "line.%zu", i + 1U);
        if (written < 0 || (size_t)written >= sizeof lineId) return UMI_STATUS_INTERNAL_ERROR;
        status = umi_accounting_journal_line_init(&line, lineId, command->lines[i].accountId.value,
            command->lines[i].debitMinor, command->lines[i].creditMinor);
        if (status == UMI_STATUS_OK) status = umi_accounting_journal_entry_add_line(&journal.entry, &line);
    }
    if (status == UMI_STATUS_OK) status = ValidateJournal(state, &journal);
    if (status == UMI_STATUS_OK) state->journals[state->counts.journals++] = journal;
    return status;
}

static UmiStatus PrepareReversal(FinanceState *state, const UmiFinanceOperationCommand *command)
{
    const size_t originalIndex = FinanceJournalIndex(state, command->referenceId);
    UmiFinanceOperationJournal journal;
    const UmiFinanceOperationJournal *original;
    UmiStatus status;
    if (originalIndex == FINANCE_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    if (FinanceJournalIndex(state, command->id) != FINANCE_INDEX_NONE) return UMI_STATUS_ALREADY_EXISTS;
    if (state->counts.journals >= UMI_FINANCE_OPERATIONS_JOURNALS) return UMI_STATUS_CAPACITY_EXCEEDED;
    original = &state->journals[originalIndex];
    if (original->settlement || original->entry.status != UMI_ACCOUNTING_JOURNAL_POSTED ||
        original->reversedBy.value[0] != '\0' || original->reverses.value[0] != '\0' ||
        umi_financial_date_compare(command->date, original->entry.accounting_date) < 0)
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < state->counts.journals; ++i)
        if (FinanceIdEqual(state->journals[i].reverses, command->referenceId)) return UMI_STATUS_ALREADY_EXISTS;
    journal = *original;
    journal.entry.id = command->id;
    journal.entry.accounting_date = command->date;
    journal.entry.status = UMI_ACCOUNTING_JOURNAL_DRAFT;
    journal.periodId = command->periodId;
    journal.makerId = command->actorId;
    memset(&journal.checkerId, 0, sizeof journal.checkerId);
    journal.reverses = original->entry.id;
    journal.postingRevision = 0U;
    for (size_t i = 0U; i < journal.entry.line_count; ++i) {
        int64_t debit = journal.entry.lines[i].debit_minor;
        journal.entry.lines[i].debit_minor = journal.entry.lines[i].credit_minor;
        journal.entry.lines[i].credit_minor = debit;
    }
    status = ValidateJournal(state, &journal);
    if (status == UMI_STATUS_OK) state->journals[state->counts.journals++] = journal;
    return status;
}

UmiStatus FinancePostJournal(FinanceState *state, UmiFinanceOperationJournal *journal)
{
    UmiStatus status = ValidateJournal(state, journal);
    size_t originalIndex = FINANCE_INDEX_NONE;
    if (status != UMI_STATUS_OK) return status;
    if (journal->entry.status != UMI_ACCOUNTING_JOURNAL_VALIDATED) return UMI_STATUS_INVALID_STATE;
    if (journal->reverses.value[0] != '\0') {
        originalIndex = FinanceJournalIndex(state, journal->reverses);
        if (originalIndex == FINANCE_INDEX_NONE || state->journals[originalIndex].settlement ||
            state->journals[originalIndex].entry.status != UMI_ACCOUNTING_JOURNAL_POSTED ||
            state->journals[originalIndex].reversedBy.value[0] != '\0') return UMI_STATUS_INVALID_STATE;
    }
    for (size_t i = 0U; i < journal->entry.line_count; ++i) {
        const UmiAccountingJournalLine *line = &journal->entry.lines[i];
        UmiFinanceOperationAccount *account = &state->accounts[FinanceAccountIndex(state, line->account_id)];
        status = FinanceAdd(account->debitMinor, line->debit_minor, &account->debitMinor);
        if (status == UMI_STATUS_OK) status = FinanceAdd(account->creditMinor, line->credit_minor, &account->creditMinor);
        if (status != UMI_STATUS_OK) return status;
        account->lastPostingRevision = state->counts.revision + 1U;
    }
    journal->entry.status = UMI_ACCOUNTING_JOURNAL_POSTED;
    journal->postingRevision = state->counts.revision + 1U;
    if (originalIndex != FINANCE_INDEX_NONE) {
        state->journals[originalIndex].reversedBy = journal->entry.id;
        state->journals[originalIndex].entry.status = UMI_ACCOUNTING_JOURNAL_REVERSED;
    }
    return UMI_STATUS_OK;
}

static UmiStatus ApproveOrPost(FinanceState *state, const UmiFinanceOperationCommand *command)
{
    const size_t index = FinanceJournalIndex(state, command->id);
    UmiFinanceOperationJournal *journal;
    UmiStatus status;
    if (index == FINANCE_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    journal = &state->journals[index];
    if (journal->settlement) return UMI_STATUS_INVALID_STATE;
    if (command->kind == UMI_FINANCE_POST_JOURNAL) {
        if (!FinanceIdEqual(journal->checkerId, command->actorId)) return UMI_STATUS_PERMISSION_DENIED;
        return FinancePostJournal(state, journal);
    }
    if (journal->entry.status != UMI_ACCOUNTING_JOURNAL_DRAFT) return UMI_STATUS_INVALID_STATE;
    if (FinanceIdEqual(journal->makerId, command->actorId)) return UMI_STATUS_PERMISSION_DENIED;
    status = ValidateJournal(state, journal);
    if (status != UMI_STATUS_OK) return status;
    journal->checkerId = command->actorId;
    journal->entry.status = UMI_ACCOUNTING_JOURNAL_VALIDATED;
    return UMI_STATUS_OK;
}

static UmiStatus Reconcile(FinanceState *state, const UmiFinanceOperationCommand *command)
{
    const size_t accountIndex = FinanceAccountIndex(state, command->accountId);
    UmiFinanceOperationReconciliation evidence;
    UmiStatus status;
    if (accountIndex == FINANCE_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    for (size_t i = 0U; i < state->counts.reconciliations; ++i)
        if (FinanceIdEqual(state->reconciliations[i].id, command->id)) return UMI_STATUS_ALREADY_EXISTS;
    if (state->counts.reconciliations >= UMI_FINANCE_OPERATIONS_RECONCILIATIONS) return UMI_STATUS_CAPACITY_EXCEEDED;
    memset(&evidence, 0, sizeof evidence);
    status = FinanceAccountBalance(state, command->accountId, &evidence.ledgerMinor);
    if (status != UMI_STATUS_OK) return status;
    evidence.id = command->id;
    evidence.accountId = command->accountId;
    evidence.actorId = command->actorId;
    evidence.externalMinor = command->amountMinor;
    evidence.matched = evidence.ledgerMinor == evidence.externalMinor;
    evidence.postingRevision = state->accounts[accountIndex].lastPostingRevision;
    evidence.revision = state->counts.revision + 1U;
    state->reconciliations[state->counts.reconciliations++] = evidence;
    return UMI_STATUS_OK;
}

/* Preparing a close freezes the period, not the whole application. Future
 * periods may continue. The close checks dated unfinished work plus current
 * reconciliation evidence for every posted account; no old evidence is erased. */
static UmiStatus CloseReady(const FinanceState *state, size_t periodIndex)
{
    const UmiFinanceOperationPeriod *period = &state->periods[periodIndex];
    for (size_t i = 0U; i < state->counts.periods; ++i)
        if (i != periodIndex && umi_financial_date_compare(state->periods[i].endDate, period->startDate) < 0 &&
            state->periods[i].status != UMI_ACCOUNTING_PERIOD_CLOSED) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < state->counts.journals; ++i) {
        const UmiFinanceOperationJournal *journal = &state->journals[i];
        if (FinanceIdEqual(journal->periodId, period->id) &&
            journal->entry.status != UMI_ACCOUNTING_JOURNAL_POSTED &&
            journal->entry.status != UMI_ACCOUNTING_JOURNAL_REVERSED) return UMI_STATUS_INVALID_STATE;
    }
    for (size_t i = 0U; i < state->counts.orders; ++i)
        if (state->orders[i].remainingLots > 0 &&
            umi_financial_date_compare(state->orders[i].date, period->endDate) <= 0) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < state->counts.fills; ++i)
        if (state->fills[i].state != UMI_SETTLEMENT_SETTLED &&
            umi_financial_date_compare(state->fills[i].tradeDate, period->endDate) <= 0) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < state->counts.accounts; ++i) {
        const UmiFinanceOperationAccount *account = &state->accounts[i];
        const UmiFinanceOperationReconciliation *latest = NULL;
        UmiFinanceTrialBalance balance;
        UmiStatus status;
        if (account->lastPostingRevision != 0U) {
            for (size_t n = 0U; n < state->counts.reconciliations; ++n)
                if (FinanceIdEqual(state->reconciliations[n].accountId, account->id)) latest = &state->reconciliations[n];
            if (latest == NULL || !latest->matched || latest->postingRevision != account->lastPostingRevision)
                return UMI_STATUS_INVALID_STATE;
        }
        status = FinanceTrialBalance(state, period->id, account->currency, account->scale, &balance);
        if (status != UMI_STATUS_OK) return status;
        if (!balance.balanced) return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

static UmiStatus ChangePeriod(FinanceState *state, const UmiFinanceOperationCommand *command)
{
    const size_t index = FinancePeriodIndex(state, command->id);
    UmiFinanceOperationPeriod *period;
    UmiStatus status;
    if (index == FINANCE_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    period = &state->periods[index];
    if (command->kind == UMI_FINANCE_REOPEN_REVIEW) {
        if (period->status != UMI_ACCOUNTING_PERIOD_SOFT_CLOSED) return UMI_STATUS_INVALID_STATE;
        period->status = UMI_ACCOUNTING_PERIOD_OPEN;
        memset(&period->preparedBy, 0, sizeof period->preparedBy);
        return UMI_STATUS_OK;
    }
    if (command->kind == UMI_FINANCE_PREPARE_CLOSE) {
        if (period->status != UMI_ACCOUNTING_PERIOD_OPEN) return UMI_STATUS_INVALID_STATE;
        status = CloseReady(state, index);
        if (status != UMI_STATUS_OK) return status;
        period->status = UMI_ACCOUNTING_PERIOD_SOFT_CLOSED;
        period->preparedBy = command->actorId;
    } else {
        if (period->status != UMI_ACCOUNTING_PERIOD_SOFT_CLOSED) return UMI_STATUS_INVALID_STATE;
        if (FinanceIdEqual(period->preparedBy, command->actorId)) return UMI_STATUS_PERMISSION_DENIED;
        status = CloseReady(state, index);
        if (status != UMI_STATUS_OK) return status;
        period->status = UMI_ACCOUNTING_PERIOD_CLOSED;
        period->closedBy = command->actorId;
    }
    return UMI_STATUS_OK;
}

UmiStatus FinanceApplyAccounting(FinanceState *state, const UmiFinanceOperationCommand *command)
{
    switch (command->kind) {
    case UMI_FINANCE_CREATE_ACCOUNT: return CreateAccount(state, command);
    case UMI_FINANCE_OPEN_PERIOD: return OpenPeriod(state, command);
    case UMI_FINANCE_ENTER_JOURNAL: return EnterJournal(state, command);
    case UMI_FINANCE_APPROVE_JOURNAL:
    case UMI_FINANCE_POST_JOURNAL: return ApproveOrPost(state, command);
    case UMI_FINANCE_REVERSE_JOURNAL: return PrepareReversal(state, command);
    case UMI_FINANCE_RECONCILE_ACCOUNT: return Reconcile(state, command);
    case UMI_FINANCE_PREPARE_CLOSE:
    case UMI_FINANCE_CLOSE_PERIOD:
    case UMI_FINANCE_REOPEN_REVIEW: return ChangePeriod(state, command);
    default: return UMI_STATUS_INVALID_ARGUMENT;
    }
}
