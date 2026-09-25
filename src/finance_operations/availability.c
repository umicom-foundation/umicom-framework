/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance_operations/availability.c
 *
 * PURPOSE:
 *   Project posted cash and custody lots with reservations from open orders and unsettled fills.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <string.h>

UmiStatus FinanceAccountBalance(const FinanceState *state, UmiFinancialId id, int64_t *out)
{
    const size_t index = FinanceAccountIndex(state, id);
    const UmiFinanceOperationAccount *account;
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index == FINANCE_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    account = &state->accounts[index];
    if (account->accountClass == UMI_ACCOUNTING_ASSET || account->accountClass == UMI_ACCOUNTING_EXPENSE)
        *out = account->debitMinor - account->creditMinor;
    else *out = account->creditMinor - account->debitMinor;
    return UMI_STATUS_OK;
}

UmiStatus FinanceCash(const FinanceState *state, UmiFinancialId participantId, UmiFinanceOperationAvailability *out)
{
    UmiFinanceOperationAvailability result = {0, 0, 0};
    size_t participant = FinanceParticipantIndex(state, participantId);
    UmiStatus status;
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (participant == FINANCE_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    status = FinanceAccountBalance(state, state->participants[participant].cashAccountId, &result.booked);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < state->counts.orders; ++i) {
        const UmiFinanceOperationOrder *order = &state->orders[i];
        size_t instrument;
        int64_t reservation;
        if (order->side != UMI_SIDE_BUY || order->remainingLots == 0 ||
            !FinanceIdEqual(order->participantId, participantId)) continue;
        instrument = FinanceInstrumentIndex(state, order->instrumentId);
        if (instrument == FINANCE_INDEX_NONE) return UMI_STATUS_INTERNAL_ERROR;
        status = FinanceCost(&state->instruments[instrument], order->limitTicks, order->remainingLots, &reservation);
        if (status == UMI_STATUS_OK) status = FinanceAdd(result.reserved, reservation, &result.reserved);
        if (status != UMI_STATUS_OK) return status;
    }
    for (size_t i = 0U; i < state->counts.fills; ++i) {
        const UmiFinanceOperationFill *fill = &state->fills[i];
        if (fill->state == UMI_SETTLEMENT_SETTLED || !FinanceIdEqual(fill->buyerId, participantId)) continue;
        status = FinanceAdd(result.reserved, fill->cash.minor_units, &result.reserved);
        if (status != UMI_STATUS_OK) return status;
    }
    if (result.booked < 0 || result.reserved > result.booked) return UMI_STATUS_INVALID_STATE;
    result.available = result.booked - result.reserved;
    *out = result;
    return UMI_STATUS_OK;
}

UmiStatus FinanceLots(const FinanceState *state, UmiFinancialId participantId,
    UmiFinancialId instrumentId, UmiFinanceOperationAvailability *out)
{
    UmiFinanceOperationAvailability result = {0, 0, 0};
    size_t holding;
    UmiStatus status;
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (FinanceParticipantIndex(state, participantId) == FINANCE_INDEX_NONE ||
        FinanceInstrumentIndex(state, instrumentId) == FINANCE_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    holding = FinanceHoldingIndex(state, participantId, instrumentId);
    if (holding != FINANCE_INDEX_NONE) result.booked = state->holdings[holding].lots;
    for (size_t i = 0U; i < state->counts.orders; ++i) {
        const UmiFinanceOperationOrder *order = &state->orders[i];
        if (order->side != UMI_SIDE_SELL || !FinanceIdEqual(order->participantId, participantId) ||
            !FinanceIdEqual(order->instrumentId, instrumentId)) continue;
        status = FinanceAdd(result.reserved, order->remainingLots, &result.reserved);
        if (status != UMI_STATUS_OK) return status;
    }
    for (size_t i = 0U; i < state->counts.fills; ++i) {
        const UmiFinanceOperationFill *fill = &state->fills[i];
        if (fill->state == UMI_SETTLEMENT_SETTLED || !FinanceIdEqual(fill->sellerId, participantId) ||
            !FinanceIdEqual(fill->instrumentId, instrumentId)) continue;
        status = FinanceAdd(result.reserved, fill->lots, &result.reserved);
        if (status != UMI_STATUS_OK) return status;
    }
    if (result.booked < 0 || result.reserved > result.booked) return UMI_STATUS_INVALID_STATE;
    result.available = result.booked - result.reserved;
    *out = result;
    return UMI_STATUS_OK;
}

/* A journal withdrawal, correction or settlement must not spend money already
 * promised by the order book. This invariant is checked before every commit. */
UmiStatus FinanceAllReservationsValid(const FinanceState *state)
{
    UmiFinanceOperationAvailability availability;
    UmiStatus status;
    for (size_t i = 0U; i < state->counts.participants; ++i) {
        status = FinanceCash(state, state->participants[i].id, &availability);
        if (status != UMI_STATUS_OK) return status;
    }
    for (size_t i = 0U; i < state->counts.holdings; ++i) {
        status = FinanceLots(state, state->holdings[i].participantId,
            state->holdings[i].instrumentId, &availability);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
