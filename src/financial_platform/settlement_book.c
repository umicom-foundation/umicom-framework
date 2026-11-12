/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/settlement_book.c
 *
 * PURPOSE:
 *   Implement bounded settlement-control lifecycle and overdue inspection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/financial_platform/settlement_book.h"
#include <string.h>

#include "control_query.h"

/* Retained for review: the former owner trusted public array bounds and inline
 * identities. Framework now admits every active row before lookup or mutation,
 * retains existing lifecycle rules and publishes checked observations atomically. */
#if 0
void umi_financial_settlement_book_init(UmiFinancialSettlementBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

const UmiFinancialSettlementControl *umi_financial_settlement_book_find(
    const UmiFinancialSettlementBook *book,
    const char *settlementId)
{
    size_t i;
    if (book == NULL || settlementId == NULL) return NULL;
    for (i = 0U; i < book->count; ++i)
        if (strcmp(book->records[i].settlementId, settlementId) == 0)
            return &book->records[i];
    return NULL;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_financial_settlement_book_add(
    UmiFinancialSettlementBook *book,
    const UmiFinancialSettlementControl *record)
{
    if (book == NULL || record == NULL || record->settlementId[0] == '\0' ||
        record->tradeOrPaymentId[0] == '\0' || record->accountId[0] == '\0' ||
        record->currency[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_financial_settlement_book_find(book, record->settlementId) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (book->count >= UMI_FINANCIAL_SETTLEMENT_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    book->records[book->count++] = *record;
    book->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_financial_settlement_book_add(
    UmiFinancialSettlementBook *book,
    const UmiFinancialSettlementControl *record)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    if (book == NULL || record == NULL || record->settlementId[0] == '\0' ||
        record->tradeOrPaymentId[0] == '\0' || record->accountId[0] == '\0' ||
        record->currency[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_financial_settlement_book_find(book, record->settlementId) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (book->count >= UMI_FINANCIAL_SETTLEMENT_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    book->records[book->count++] = *record;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_financial_settlement_book_transition(
    UmiFinancialSettlementBook *book,
    const char *settlementId,
    UmiFinancialSettlementState state,
    uint64_t version)
{
    size_t i;
    if (book == NULL || settlementId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < book->count; ++i) {
        UmiFinancialSettlementControl *record = &book->records[i];
        if (strcmp(record->settlementId, settlementId) == 0) {
            if (version <= record->version) return UMI_STATUS_INVALID_STATE;
            if (record->state == UMI_FINANCIAL_SETTLEMENT_SETTLED)
                return UMI_STATUS_INVALID_STATE;
            record->state = state;
            record->version = version;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
#endif

UmiStatus umi_financial_settlement_book_transition(
    UmiFinancialSettlementBook *book,
    const char *settlementId,
    UmiFinancialSettlementState state,
    uint64_t version)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    size_t i;
    if (book == NULL || settlementId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < book->count; ++i) {
        UmiFinancialSettlementControl *record = &book->records[i];
        if (strcmp(record->settlementId, settlementId) == 0) {
            if (version <= record->version) return UMI_STATUS_INVALID_STATE;
            if (record->state == UMI_FINANCIAL_SETTLEMENT_SETTLED)
                return UMI_STATUS_INVALID_STATE;
            record->state = state;
            record->version = version;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

size_t umi_financial_settlement_book_overdue_count(
    const UmiFinancialSettlementBook *book,
    uint64_t nowMilliseconds)
{
    size_t i, count = 0U;
    if (book == NULL) return 0U;
    for (i = 0U; i < book->count; ++i) {
        const UmiFinancialSettlementControl *record = &book->records[i];
        if (record->state != UMI_FINANCIAL_SETTLEMENT_SETTLED &&
            record->dueMilliseconds > 0U &&
            nowMilliseconds > record->dueMilliseconds) {
            count += 1U;
        }
    }
    return count;
}

#endif

UmiStatus umi_financial_settlement_control_validate(const UmiFinancialSettlementControl *row)
{
    static const UmiSnapshotTextField fields[] = {
        {"settlementId", offsetof(UmiFinancialSettlementControl, settlementId), sizeof(((UmiFinancialSettlementControl *)0)->settlementId), 1},
        {"tradeOrPaymentId", offsetof(UmiFinancialSettlementControl, tradeOrPaymentId), sizeof(((UmiFinancialSettlementControl *)0)->tradeOrPaymentId), 1},
        {"accountId", offsetof(UmiFinancialSettlementControl, accountId), sizeof(((UmiFinancialSettlementControl *)0)->accountId), 1},
        {"currency", offsetof(UmiFinancialSettlementControl, currency), sizeof(((UmiFinancialSettlementControl *)0)->currency), 1}
    };
    if (row == NULL || UmiSnapshotValidateTextFields(row, sizeof(*row), fields,
            sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK ||
        !isfinite(row->cashAmount) ||
        !isfinite(row->securitiesQuantity) ||
        (row->state < UMI_FINANCIAL_SETTLEMENT_PENDING || row->state > UMI_FINANCIAL_SETTLEMENT_FAILED)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_financial_settlement_book_validate(const UmiFinancialSettlementBook *owner)
{
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->count > UMI_FINANCIAL_SETTLEMENT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (owner->revision == 0U) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < owner->count; ++i) {
        const UmiFinancialSettlementControl *row = &owner->records[i];
        if (umi_financial_settlement_control_validate(row) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j) {
            const UmiFinancialSettlementControl *previous = &owner->records[j];
            if (strcmp(row->settlementId, previous->settlementId) == 0) return UMI_STATUS_INVALID_STATE;
        }
    }
    return UMI_STATUS_OK;
}
void umi_financial_settlement_book_init(UmiFinancialSettlementBook *owner)
{
    if (owner == NULL) return;
    memset(owner, 0, sizeof(*owner));
    owner->revision = 1U;
}
const UmiFinancialSettlementControl *umi_financial_settlement_book_find(const UmiFinancialSettlementBook *owner,
    const char *settlementId)
{
    char settlementIdKey[64];
    if (FinancialControlQuery(settlementId, settlementIdKey, sizeof settlementIdKey) != UMI_STATUS_OK) return NULL;
    if (umi_financial_settlement_book_validate(owner) != UMI_STATUS_OK) return NULL;
    for (size_t i = 0U; i < owner->count; ++i) {
        const UmiFinancialSettlementControl *row = &owner->records[i];
        if (strcmp(row->settlementId, settlementIdKey) == 0) return row;
    }
    return NULL;
}
UmiStatus umi_financial_settlement_book_add(UmiFinancialSettlementBook *owner, const UmiFinancialSettlementControl *row)
{
    if (owner != NULL && owner->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_financial_settlement_book_validate(owner);
    if (status == UMI_STATUS_OK) status = umi_financial_settlement_control_validate(row);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < owner->count; ++i) {
        UmiFinancialSettlementControl *current = &owner->records[i];
        if (strcmp(current->settlementId, row->settlementId) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (owner->count == UMI_FINANCIAL_SETTLEMENT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    owner->records[owner->count++] = *row;
    ++owner->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_financial_settlement_book_transition(UmiFinancialSettlementBook *owner, const char *settlementId,
    UmiFinancialSettlementState state, uint64_t version)
{
    if (owner != NULL && owner->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (state < UMI_FINANCIAL_SETTLEMENT_PENDING || state > UMI_FINANCIAL_SETTLEMENT_FAILED) return UMI_STATUS_INVALID_ARGUMENT;
    char key[64];
    UmiStatus status = FinancialControlQuery(settlementId, key, sizeof key);
    if (status == UMI_STATUS_OK) status = umi_financial_settlement_book_validate(owner);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < owner->count; ++i) {
        UmiFinancialSettlementControl *row = &owner->records[i];
        if (strcmp(row->settlementId, key) == 0) {
            if (version <= row->version || row->state == UMI_FINANCIAL_SETTLEMENT_SETTLED)
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

UmiStatus umi_financial_settlement_book_overdue_count_checked(
    const UmiFinancialSettlementBook *book, uint64_t nowMilliseconds, size_t *outCount)
{
    if (outCount == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_financial_settlement_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    size_t count = 0U;
    for (size_t i = 0U; i < book->count; ++i) {
        const UmiFinancialSettlementControl *row = &book->records[i];
        if (row->state != UMI_FINANCIAL_SETTLEMENT_SETTLED && row->dueMilliseconds != 0U &&
            nowMilliseconds > row->dueMilliseconds) ++count;
    }
    *outCount = count;
    return UMI_STATUS_OK;
}
size_t umi_financial_settlement_book_overdue_count(
    const UmiFinancialSettlementBook *book, uint64_t nowMilliseconds)
{
    size_t count;
    return umi_financial_settlement_book_overdue_count_checked(book, nowMilliseconds, &count) == UMI_STATUS_OK ? count : 0U;
}
