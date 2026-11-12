/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/trade_control_book.c
 *
 * PURPOSE:
 *   Implement monotonic trade-control lifecycle references.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/financial_platform/trade_control_book.h"
#include <string.h>

#include "control_query.h"

/* Retained for review: the former owner trusted public array bounds and inline
 * identities. Framework now admits every active row before lookup or mutation,
 * retains existing lifecycle rules and publishes checked observations atomically. */
#if 0
void umi_financial_trade_control_book_init(UmiFinancialTradeControlBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

const UmiFinancialTradeControl *umi_financial_trade_control_book_find(
    const UmiFinancialTradeControlBook *book,
    const char *tradeId)
{
    size_t i;
    if (book == NULL || tradeId == NULL) return NULL;
    for (i = 0U; i < book->count; ++i)
        if (strcmp(book->trades[i].tradeId, tradeId) == 0)
            return &book->trades[i];
    return NULL;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_financial_trade_control_book_add(
    UmiFinancialTradeControlBook *book,
    const UmiFinancialTradeControl *trade)
{
    if (book == NULL || trade == NULL || trade->tradeId[0] == '\0' ||
        trade->accountId[0] == '\0' || trade->counterpartyId[0] == '\0' ||
        trade->instrumentId[0] == '\0' || trade->currency[0] == '\0' ||
        trade->quantity == 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_financial_trade_control_book_find(book, trade->tradeId) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (book->count >= UMI_FINANCIAL_TRADE_CONTROL_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    book->trades[book->count++] = *trade;
    book->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_financial_trade_control_book_add(
    UmiFinancialTradeControlBook *book,
    const UmiFinancialTradeControl *trade)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    if (book == NULL || trade == NULL || trade->tradeId[0] == '\0' ||
        trade->accountId[0] == '\0' || trade->counterpartyId[0] == '\0' ||
        trade->instrumentId[0] == '\0' || trade->currency[0] == '\0' ||
        trade->quantity == 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_financial_trade_control_book_find(book, trade->tradeId) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (book->count >= UMI_FINANCIAL_TRADE_CONTROL_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    book->trades[book->count++] = *trade;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_financial_trade_control_book_transition(
    UmiFinancialTradeControlBook *book,
    const char *tradeId,
    UmiFinancialTradeControlState state,
    uint64_t version)
{
    size_t i;
    if (book == NULL || tradeId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < book->count; ++i) {
        UmiFinancialTradeControl *trade = &book->trades[i];
        if (strcmp(trade->tradeId, tradeId) == 0) {
            if (version <= trade->version) return UMI_STATUS_INVALID_STATE;
            if (trade->state == UMI_FINANCIAL_TRADE_CANCELLED ||
                trade->state == UMI_FINANCIAL_TRADE_SETTLED)
                return UMI_STATUS_INVALID_STATE;
            trade->state = state;
            trade->version = version;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
#endif

UmiStatus umi_financial_trade_control_book_transition(
    UmiFinancialTradeControlBook *book,
    const char *tradeId,
    UmiFinancialTradeControlState state,
    uint64_t version)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    size_t i;
    if (book == NULL || tradeId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < book->count; ++i) {
        UmiFinancialTradeControl *trade = &book->trades[i];
        if (strcmp(trade->tradeId, tradeId) == 0) {
            if (version <= trade->version) return UMI_STATUS_INVALID_STATE;
            if (trade->state == UMI_FINANCIAL_TRADE_CANCELLED ||
                trade->state == UMI_FINANCIAL_TRADE_SETTLED)
                return UMI_STATUS_INVALID_STATE;
            trade->state = state;
            trade->version = version;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

#endif

UmiStatus umi_financial_trade_control_validate(const UmiFinancialTradeControl *row)
{
    static const UmiSnapshotTextField fields[] = {
        {"tradeId", offsetof(UmiFinancialTradeControl, tradeId), sizeof(((UmiFinancialTradeControl *)0)->tradeId), 1},
        {"accountId", offsetof(UmiFinancialTradeControl, accountId), sizeof(((UmiFinancialTradeControl *)0)->accountId), 1},
        {"counterpartyId", offsetof(UmiFinancialTradeControl, counterpartyId), sizeof(((UmiFinancialTradeControl *)0)->counterpartyId), 1},
        {"instrumentId", offsetof(UmiFinancialTradeControl, instrumentId), sizeof(((UmiFinancialTradeControl *)0)->instrumentId), 1},
        {"currency", offsetof(UmiFinancialTradeControl, currency), sizeof(((UmiFinancialTradeControl *)0)->currency), 1}
    };
    if (row == NULL || UmiSnapshotValidateTextFields(row, sizeof(*row), fields,
            sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK ||
        !isfinite(row->quantity) ||
        !isfinite(row->price) ||
        !isfinite(row->notional) ||
        row->quantity == 0.0 ||
        (row->state < UMI_FINANCIAL_TRADE_DRAFT || row->state > UMI_FINANCIAL_TRADE_CANCELLED)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_financial_trade_control_book_validate(const UmiFinancialTradeControlBook *owner)
{
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->count > UMI_FINANCIAL_TRADE_CONTROL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (owner->revision == 0U) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < owner->count; ++i) {
        const UmiFinancialTradeControl *row = &owner->trades[i];
        if (umi_financial_trade_control_validate(row) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j) {
            const UmiFinancialTradeControl *previous = &owner->trades[j];
            if (strcmp(row->tradeId, previous->tradeId) == 0) return UMI_STATUS_INVALID_STATE;
        }
    }
    return UMI_STATUS_OK;
}
void umi_financial_trade_control_book_init(UmiFinancialTradeControlBook *owner)
{
    if (owner == NULL) return;
    memset(owner, 0, sizeof(*owner));
    owner->revision = 1U;
}
const UmiFinancialTradeControl *umi_financial_trade_control_book_find(const UmiFinancialTradeControlBook *owner,
    const char *tradeId)
{
    char tradeIdKey[64];
    if (FinancialControlQuery(tradeId, tradeIdKey, sizeof tradeIdKey) != UMI_STATUS_OK) return NULL;
    if (umi_financial_trade_control_book_validate(owner) != UMI_STATUS_OK) return NULL;
    for (size_t i = 0U; i < owner->count; ++i) {
        const UmiFinancialTradeControl *row = &owner->trades[i];
        if (strcmp(row->tradeId, tradeIdKey) == 0) return row;
    }
    return NULL;
}
UmiStatus umi_financial_trade_control_book_add(UmiFinancialTradeControlBook *owner, const UmiFinancialTradeControl *row)
{
    if (owner != NULL && owner->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_financial_trade_control_book_validate(owner);
    if (status == UMI_STATUS_OK) status = umi_financial_trade_control_validate(row);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < owner->count; ++i) {
        UmiFinancialTradeControl *current = &owner->trades[i];
        if (strcmp(current->tradeId, row->tradeId) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (owner->count == UMI_FINANCIAL_TRADE_CONTROL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    owner->trades[owner->count++] = *row;
    ++owner->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_financial_trade_control_book_transition(UmiFinancialTradeControlBook *owner, const char *tradeId,
    UmiFinancialTradeControlState state, uint64_t version)
{
    if (owner != NULL && owner->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (state < UMI_FINANCIAL_TRADE_DRAFT || state > UMI_FINANCIAL_TRADE_CANCELLED) return UMI_STATUS_INVALID_ARGUMENT;
    char key[64];
    UmiStatus status = FinancialControlQuery(tradeId, key, sizeof key);
    if (status == UMI_STATUS_OK) status = umi_financial_trade_control_book_validate(owner);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < owner->count; ++i) {
        UmiFinancialTradeControl *row = &owner->trades[i];
        if (strcmp(row->tradeId, key) == 0) {
            if (version <= row->version || row->state == UMI_FINANCIAL_TRADE_CANCELLED || row->state == UMI_FINANCIAL_TRADE_SETTLED)
                return UMI_STATUS_INVALID_STATE;
            /* Preserve known-state transitions and existing terminal rules.
             * Updating this observation performs no external financial action. */
            row->state = state;
            row->version = version;
            ++owner->revision;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
