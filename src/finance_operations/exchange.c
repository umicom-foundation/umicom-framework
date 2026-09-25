/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance_operations/exchange.c
 *
 * PURPOSE:
 *   Sequence cash-backed limit orders, preserve price/time priority and retain every accepted order and fill.
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

static UmiStatus RegisterParticipant(FinanceState *state,
    const UmiFinanceOperationCommand *command)
{
    size_t accountIndex = FinanceAccountIndex(state, command->accountId);
    UmiFinanceOperationParticipant *participant;
    if (FinanceParticipantIndex(state, command->id) != FINANCE_INDEX_NONE)
        return UMI_STATUS_ALREADY_EXISTS;
    if (!FinanceTextValid(command->name, sizeof(command->name), true) ||
        accountIndex == FINANCE_INDEX_NONE) return UMI_STATUS_INVALID_ARGUMENT;
    if (state->accounts[accountIndex].accountClass != UMI_ACCOUNTING_LIABILITY)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < state->counts.participants; ++index) {
        if (FinanceIdEqual(state->participants[index].cashAccountId, command->accountId))
            return UMI_STATUS_ALREADY_EXISTS;
    }
    if (state->counts.participants >= UMI_FINANCE_OPERATIONS_PARTICIPANTS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    participant = &state->participants[state->counts.participants++];
    memset(participant, 0, sizeof(*participant));
    participant->id = command->id;
    participant->cashAccountId = command->accountId;
    memcpy(participant->name, command->name, sizeof(participant->name));
    return UMI_STATUS_OK;
}

static UmiStatus ListInstrument(FinanceState *state,
    const UmiFinanceOperationCommand *command)
{
    UmiFinanceOperationInstrument *instrument;
    if (FinanceInstrumentIndex(state, command->id) != FINANCE_INDEX_NONE)
        return UMI_STATUS_ALREADY_EXISTS;
    if (!FinanceTextValid(command->name, sizeof(command->name), true) ||
        !FinanceCurrencyValid(command->currency) || command->minorPerTick <= 0 ||
        command->unitsPerLot <= 0 || command->maxOrderLots <= 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < state->counts.accounts; ++index) {
        if (umi_accounting_currency_equal(state->accounts[index].currency, command->currency) &&
            state->accounts[index].scale != command->scale) return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (size_t index = 0U; index < state->counts.instruments; ++index) {
        if (umi_accounting_currency_equal(state->instruments[index].currency, command->currency) &&
            state->instruments[index].scale != command->scale) return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (state->counts.instruments >= UMI_FINANCE_OPERATIONS_INSTRUMENTS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    instrument = &state->instruments[state->counts.instruments++];
    memset(instrument, 0, sizeof(*instrument));
    instrument->id = command->id;
    memcpy(instrument->name, command->name, sizeof(instrument->name));
    instrument->currency = command->currency;
    instrument->scale = command->scale;
    instrument->minorPerTick = command->minorPerTick;
    instrument->unitsPerLot = command->unitsPerLot;
    instrument->maxOrderLots = command->maxOrderLots;
    /* Listing is not permission to trade. Opening the market is a separate,
     * retained command so a newly listed instrument cannot match by accident. */
    instrument->marketState = UMI_MARKET_HALTED;
    return UMI_STATUS_OK;
}

static UmiStatus DepositLots(FinanceState *state,
    const UmiFinanceOperationCommand *command)
{
    size_t index;
    int64_t nextLots;
    UmiStatus status;
    if (FinanceParticipantIndex(state, command->id) == FINANCE_INDEX_NONE ||
        FinanceInstrumentIndex(state, command->instrumentId) == FINANCE_INDEX_NONE ||
        command->lots <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    index = FinanceHoldingIndex(state, command->id, command->instrumentId);
    if (index == FINANCE_INDEX_NONE) {
        if (state->counts.holdings >= UMI_FINANCE_OPERATIONS_HOLDINGS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = state->counts.holdings++;
        state->holdings[index].participantId = command->id;
        state->holdings[index].instrumentId = command->instrumentId;
        state->holdings[index].lots = 0;
    }
    status = FinanceAdd(state->holdings[index].lots, command->lots, &nextLots);
    if (status != UMI_STATUS_OK) return status;
    /* This explicit simulation injection represents custody, not an asset
     * owned by the exchange. It does not invent a cash or company-asset posting. */
    state->holdings[index].lots = nextLots;
    return UMI_STATUS_OK;
}

static bool DateHasOpenPeriod(const FinanceState *state, UmiFinancialDate date)
{
    for (size_t index = 0U; index < state->counts.periods; ++index) {
        if (state->periods[index].status == UMI_ACCOUNTING_PERIOD_OPEN &&
            FinanceDateInPeriod(date, &state->periods[index])) return true;
    }
    return false;
}

static size_t BestRestingOrder(const FinanceState *state, size_t incomingIndex)
{
    const UmiFinanceOperationOrder *incoming = &state->orders[incomingIndex];
    size_t best = FINANCE_INDEX_NONE;
    for (size_t index = 0U; index < incomingIndex; ++index) {
        const UmiFinanceOperationOrder *candidate = &state->orders[index];
        bool better;
        if (candidate->remainingLots <= 0 || candidate->side == incoming->side ||
            !FinanceIdEqual(candidate->instrumentId, incoming->instrumentId)) continue;
        if (incoming->side == UMI_SIDE_BUY && candidate->limitTicks > incoming->limitTicks) continue;
        if (incoming->side == UMI_SIDE_SELL && candidate->limitTicks < incoming->limitTicks) continue;
        if (best == FINANCE_INDEX_NONE) { best = index; continue; }
        better = incoming->side == UMI_SIDE_BUY
            ? candidate->limitTicks < state->orders[best].limitTicks
            : candidate->limitTicks > state->orders[best].limitTicks;
        if (better || (candidate->limitTicks == state->orders[best].limitTicks &&
            candidate->sequence < state->orders[best].sequence)) best = index;
    }
    return best;
}

static void UpdateOrderStatus(UmiFinanceOperationOrder *order)
{
    order->status = order->remainingLots == 0 ? UMI_ORDER_FILLED :
        (order->filledLots > 0 ? UMI_ORDER_PARTIALLY_FILLED : UMI_ORDER_ACCEPTED);
}

static UmiStatus MatchOrders(FinanceState *state, size_t incomingIndex,
    const UmiFinanceOperationInstrument *instrument)
{
    UmiFinanceOperationOrder *incoming = &state->orders[incomingIndex];
    UmiTradingMatchingPolicy policy;
    UmiStatus status = umi_trading_matching_policy_init(&policy, true, true,
        UMI_TRADING_CORE_MAX_EVENTS);
    if (status != UMI_STATUS_OK) return status;
    while (incoming->remainingLots > 0) {
        size_t restingIndex = BestRestingOrder(state, incomingIndex);
        UmiFinanceOperationOrder *resting;
        UmiFinanceOperationOrder *buy;
        UmiFinanceOperationOrder *sell;
        UmiTradingMatchRequest request;
        UmiTradingMatchResult result;
        UmiFinanceOperationFill *fill;
        char fillId[UMI_FINANCE_ID_CAPACITY];
        int written;
        int64_t cash;
        if (restingIndex == FINANCE_INDEX_NONE) break;
        resting = &state->orders[restingIndex];
        if (FinanceIdEqual(resting->participantId, incoming->participantId))
            return UMI_STATUS_PERMISSION_DENIED;
        /* Accepted dates cannot move backwards relative to a resting order.
         * Reject the whole candidate rather than produce a backdated execution. */
        if (umi_financial_date_compare(incoming->date, resting->date) < 0)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (state->counts.fills >= UMI_FINANCE_OPERATIONS_FILLS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        buy = incoming->side == UMI_SIDE_BUY ? incoming : resting;
        sell = incoming->side == UMI_SIDE_SELL ? incoming : resting;
        request.buy_limit_ticks = buy->limitTicks;
        request.buy_lots = buy->remainingLots;
        request.sell_limit_ticks = sell->limitTicks;
        request.sell_lots = sell->remainingLots;
        status = umi_trading_matching_engine_match(&policy, &request, &result);
        if (status != UMI_STATUS_OK || !result.matched)
            return status == UMI_STATUS_OK ? UMI_STATUS_INTERNAL_ERROR : status;
        /* Reuse canonical crossing and quantity validation. The older pair
         * matcher has no arrival sequence and chooses the sell price. This
         * book supplies the missing maker/taker context and uses the resting
         * price on either side; the legacy matcher and its API remain intact. */
        status = FinanceCost(instrument, resting->limitTicks, result.execution_lots, &cash);
        if (status != UMI_STATUS_OK) return status;
        written = snprintf(fillId, sizeof(fillId), "system.fill.%06zu", state->counts.fills + 1U);
        if (written < 0 || (size_t)written >= sizeof(fillId)) return UMI_STATUS_CAPACITY_EXCEEDED;
        fill = &state->fills[state->counts.fills++];
        memset(fill, 0, sizeof(*fill));
        status = FinanceSetId(&fill->id, fillId);
        if (status != UMI_STATUS_OK) return status;
        fill->buyOrderId = buy->id;
        fill->sellOrderId = sell->id;
        fill->buyerId = buy->participantId;
        fill->sellerId = sell->participantId;
        fill->instrumentId = instrument->id;
        fill->executionTicks = resting->limitTicks;
        fill->lots = result.execution_lots;
        fill->cash.minor_units = cash;
        fill->cash.currency = instrument->currency;
        fill->cash.scale = instrument->scale;
        fill->tradeDate = incoming->date;
        fill->state = UMI_SETTLEMENT_PENDING;
        buy->remainingLots -= result.execution_lots;
        sell->remainingLots -= result.execution_lots;
        buy->filledLots += result.execution_lots;
        sell->filledLots += result.execution_lots;
        UpdateOrderStatus(buy);
        UpdateOrderStatus(sell);
    }
    return UMI_STATUS_OK;
}

static UmiStatus PlaceOrder(FinanceState *state,
    const UmiFinanceOperationCommand *command)
{
    size_t participantIndex = FinanceParticipantIndex(state, command->participantId);
    size_t instrumentIndex = FinanceInstrumentIndex(state, command->instrumentId);
    size_t cashIndex;
    size_t incomingIndex;
    UmiFinanceOperationAvailability availability;
    const UmiFinanceOperationInstrument *instrument;
    UmiFinanceOperationOrder *order;
    UmiStatus status;
    int64_t required;
    if (FinanceOrderIndex(state, command->id) != FINANCE_INDEX_NONE) return UMI_STATUS_ALREADY_EXISTS;
    if (participantIndex == FINANCE_INDEX_NONE || instrumentIndex == FINANCE_INDEX_NONE ||
        (command->side != UMI_SIDE_BUY && command->side != UMI_SIDE_SELL) ||
        command->priceTicks <= 0 || command->lots <= 0 ||
        !umi_financial_date_is_valid(command->date)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!DateHasOpenPeriod(state, command->date)) return UMI_STATUS_INVALID_STATE;
    instrument = &state->instruments[instrumentIndex];
    if (instrument->marketState != UMI_MARKET_OPEN) return UMI_STATUS_INVALID_STATE;
    if (command->lots > instrument->maxOrderLots) return UMI_STATUS_PERMISSION_DENIED;
    cashIndex = FinanceAccountIndex(state, state->participants[participantIndex].cashAccountId);
    if (cashIndex == FINANCE_INDEX_NONE) return UMI_STATUS_INTERNAL_ERROR;
    if (!umi_accounting_currency_equal(state->accounts[cashIndex].currency, instrument->currency) ||
        state->accounts[cashIndex].scale != instrument->scale) return UMI_STATUS_INVALID_ARGUMENT;
    /* Reserve the whole submitted limit, even when some quantity may execute
     * more cheaply. This prevents a partially matched order exceeding buying
     * power and gives the caller one predictable pre-trade requirement. */
    if (command->side == UMI_SIDE_BUY) {
        status = FinanceCost(instrument, command->priceTicks, command->lots, &required);
        if (status != UMI_STATUS_OK) return status;
        status = FinanceCash(state, command->participantId, &availability);
    } else {
        required = command->lots;
        status = FinanceLots(state, command->participantId, command->instrumentId, &availability);
    }
    if (status != UMI_STATUS_OK) return status;
    if (required > availability.available) return UMI_STATUS_PERMISSION_DENIED;
    if (state->counts.orders >= UMI_FINANCE_OPERATIONS_ORDERS) return UMI_STATUS_CAPACITY_EXCEEDED;
    incomingIndex = state->counts.orders++;
    order = &state->orders[incomingIndex];
    memset(order, 0, sizeof(*order));
    order->id = command->id;
    order->participantId = command->participantId;
    order->instrumentId = command->instrumentId;
    order->side = command->side;
    order->limitTicks = command->priceTicks;
    order->originalLots = command->lots;
    order->remainingLots = command->lots;
    order->date = command->date;
    order->sequence = state->counts.revision + 1U;
    order->status = UMI_ORDER_ACCEPTED;
    return MatchOrders(state, incomingIndex, instrument);
}

UmiStatus FinanceApplyExchange(FinanceState *state,
    const UmiFinanceOperationCommand *command)
{
    size_t index;
    switch (command->kind) {
    case UMI_FINANCE_REGISTER_PARTICIPANT: return RegisterParticipant(state, command);
    case UMI_FINANCE_LIST_INSTRUMENT: return ListInstrument(state, command);
    case UMI_FINANCE_DEPOSIT_LOTS: return DepositLots(state, command);
    case UMI_FINANCE_PLACE_ORDER: return PlaceOrder(state, command);
    case UMI_FINANCE_SET_MARKET_STATE:
        index = FinanceInstrumentIndex(state, command->id);
        if (index == FINANCE_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
        state->instruments[index].marketState = command->enabled ? UMI_MARKET_OPEN : UMI_MARKET_HALTED;
        return UMI_STATUS_OK;
    case UMI_FINANCE_CANCEL_ORDER:
        index = FinanceOrderIndex(state, command->id);
        if (index == FINANCE_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
        if (!FinanceIdEqual(state->orders[index].participantId, command->participantId))
            return UMI_STATUS_PERMISSION_DENIED;
        if (state->orders[index].remainingLots == 0) return UMI_STATUS_INVALID_STATE;
        state->orders[index].remainingLots = 0;
        state->orders[index].status = UMI_ORDER_CANCELLED;
        return UMI_STATUS_OK;
    default: return UMI_STATUS_INVALID_ARGUMENT;
    }
}
