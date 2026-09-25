/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance_operations/settlement.c
 *
 * PURPOSE:
 *   Commit the cash journal and custody transfer of a cleared simulated fill as one Framework operation.
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

static UmiStatus BuildSettlementJournal(FinanceState *state,
    const UmiFinanceOperationCommand *command, size_t fillIndex,
    UmiFinancialId buyerCash, UmiFinancialId sellerCash)
{
    UmiFinanceOperationFill *fill = &state->fills[fillIndex];
    UmiFinanceOperationJournal journal;
    UmiAccountingJournalLine line;
    UmiStatus status;
    char journalId[UMI_FINANCE_ID_CAPACITY];
    int written;
    if (state->counts.journals >= UMI_FINANCE_OPERATIONS_JOURNALS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    written = snprintf(journalId, sizeof(journalId), "system.settlement.%06zu", fillIndex + 1U);
    if (written < 0 || (size_t)written >= sizeof(journalId)) return UMI_STATUS_CAPACITY_EXCEEDED;
    memset(&journal, 0, sizeof(journal));
    status = umi_accounting_journal_entry_init(&journal.entry, journalId, command->date);
    if (status != UMI_STATUS_OK) return status;
    if (FinanceJournalIndex(state, journal.entry.id) != FINANCE_INDEX_NONE)
        return UMI_STATUS_ALREADY_EXISTS;
    journal.periodId = command->periodId;
    journal.makerId = fill->clearingActor;
    journal.checkerId = command->actorId;
    journal.fillId = fill->id;
    journal.settlement = true;
    journal.currency = fill->cash.currency;
    journal.scale = fill->cash.scale;
    /* A participant cash account is a liability of this local exchange book.
     * Debiting the buyer reduces that liability; crediting the seller increases
     * theirs. No external bank balance or payment-network status is invented. */
    status = umi_accounting_journal_line_init(&line, "buyer.cash", buyerCash.value,
        fill->cash.minor_units, 0);
    if (status == UMI_STATUS_OK) status = umi_accounting_journal_entry_add_line(&journal.entry, &line);
    if (status != UMI_STATUS_OK) return status;
    status = umi_accounting_journal_line_init(&line, "seller.cash", sellerCash.value,
        0, fill->cash.minor_units);
    if (status == UMI_STATUS_OK) status = umi_accounting_journal_entry_add_line(&journal.entry, &line);
    if (status != UMI_STATUS_OK) return status;
    journal.entry.status = UMI_ACCOUNTING_JOURNAL_VALIDATED;
    status = FinancePostJournal(state, &journal);
    if (status != UMI_STATUS_OK) return status;
    state->journals[state->counts.journals++] = journal;
    fill->settlementJournalId = journal.entry.id;
    return UMI_STATUS_OK;
}

static UmiStatus SettleFill(FinanceState *state,
    const UmiFinanceOperationCommand *command, size_t fillIndex)
{
    UmiFinanceOperationFill *fill = &state->fills[fillIndex];
    size_t periodIndex = FinancePeriodIndex(state, command->periodId);
    size_t buyer = FinanceParticipantIndex(state, fill->buyerId);
    size_t seller = FinanceParticipantIndex(state, fill->sellerId);
    size_t buyerHolding = FinanceHoldingIndex(state, fill->buyerId, fill->instrumentId);
    size_t sellerHolding = FinanceHoldingIndex(state, fill->sellerId, fill->instrumentId);
    int64_t nextBuyerLots;
    UmiStatus status;
    if (fill->state != UMI_SETTLEMENT_MATCHED) return UMI_STATUS_INVALID_STATE;
    if (FinanceIdEqual(fill->clearingActor, command->actorId)) return UMI_STATUS_PERMISSION_DENIED;
    if (periodIndex == FINANCE_INDEX_NONE || !umi_financial_date_is_valid(command->date) ||
        umi_financial_date_compare(command->date, fill->tradeDate) < 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (state->periods[periodIndex].status != UMI_ACCOUNTING_PERIOD_OPEN ||
        !FinanceDateInPeriod(command->date, &state->periods[periodIndex])) return UMI_STATUS_INVALID_STATE;
    if (buyer == FINANCE_INDEX_NONE || seller == FINANCE_INDEX_NONE ||
        sellerHolding == FINANCE_INDEX_NONE) return UMI_STATUS_INTERNAL_ERROR;
    if (state->holdings[sellerHolding].lots < fill->lots) return UMI_STATUS_PERMISSION_DENIED;
    if (buyerHolding == FINANCE_INDEX_NONE) {
        if (state->counts.holdings >= UMI_FINANCE_OPERATIONS_HOLDINGS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        buyerHolding = state->counts.holdings++;
        state->holdings[buyerHolding].participantId = fill->buyerId;
        state->holdings[buyerHolding].instrumentId = fill->instrumentId;
        state->holdings[buyerHolding].lots = 0;
    }
    status = FinanceAdd(state->holdings[buyerHolding].lots, fill->lots, &nextBuyerLots);
    if (status != UMI_STATUS_OK) return status;
    status = BuildSettlementJournal(state, command, fillIndex,
        state->participants[buyer].cashAccountId, state->participants[seller].cashAccountId);
    if (status != UMI_STATUS_OK) return status;
    state->holdings[sellerHolding].lots -= fill->lots;
    state->holdings[buyerHolding].lots = nextBuyerLots;
    fill->state = UMI_SETTLEMENT_SETTLED;
    fill->settlementDate = command->date;
    /* Reservation checks run after both legs and the settlement state change.
     * The Master Controller publishes this candidate only after its single
     * Data Server transaction commits, so no observer sees a half settlement. */
    return UMI_STATUS_OK;
}

UmiStatus FinanceApplySettlement(FinanceState *state,
    const UmiFinanceOperationCommand *command)
{
    size_t index = FinanceFillIndex(state, command->id);
    if (index == FINANCE_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    if (command->kind == UMI_FINANCE_CLEAR_FILL) {
        if (state->fills[index].state != UMI_SETTLEMENT_PENDING) return UMI_STATUS_INVALID_STATE;
        state->fills[index].state = UMI_SETTLEMENT_MATCHED;
        state->fills[index].clearingActor = command->actorId;
        return UMI_STATUS_OK;
    }
    if (command->kind == UMI_FINANCE_SETTLE_FILL) return SettleFill(state, command, index);
    return UMI_STATUS_INVALID_ARGUMENT;
}
