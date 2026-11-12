/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/position_control_book.c
 *
 * PURPOSE:
 *   Implement monotonic position-control snapshots and account market value.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/financial_platform/position_control_book.h"
#include <string.h>

#include "control_query.h"

/* Retained for review: the former owner trusted public array bounds and inline
 * identities. Framework now admits every active row before lookup or mutation,
 * retains existing lifecycle rules and publishes checked observations atomically. */
#if 0
void umi_financial_position_control_book_init(UmiFinancialPositionControlBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

const UmiFinancialPositionControl *umi_financial_position_control_book_find(
    const UmiFinancialPositionControlBook *book,
    const char *accountId,
    const char *instrumentId)
{
    size_t i;
    if (book == NULL || accountId == NULL || instrumentId == NULL) return NULL;
    for (i = 0U; i < book->count; ++i)
        if (strcmp(book->positions[i].accountId, accountId) == 0 &&
            strcmp(book->positions[i].instrumentId, instrumentId) == 0)
            return &book->positions[i];
    return NULL;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_financial_position_control_book_upsert(
    UmiFinancialPositionControlBook *book,
    const UmiFinancialPositionControl *position)
{
    size_t i;
    if (book == NULL || position == NULL || position->accountId[0] == '\0' ||
        position->instrumentId[0] == '\0' || position->currency[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (i = 0U; i < book->count; ++i) {
        UmiFinancialPositionControl *current = &book->positions[i];
        if (strcmp(current->accountId, position->accountId) == 0 &&
            strcmp(current->instrumentId, position->instrumentId) == 0) {
            if (position->sourceVersion < current->sourceVersion)
                return UMI_STATUS_INVALID_STATE;
            *current = *position;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    if (book->count >= UMI_FINANCIAL_POSITION_CONTROL_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    book->positions[book->count++] = *position;
    book->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_financial_position_control_book_upsert(
    UmiFinancialPositionControlBook *book,
    const UmiFinancialPositionControl *position)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    size_t i;
    if (book == NULL || position == NULL || position->accountId[0] == '\0' ||
        position->instrumentId[0] == '\0' || position->currency[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (i = 0U; i < book->count; ++i) {
        UmiFinancialPositionControl *current = &book->positions[i];
        if (strcmp(current->accountId, position->accountId) == 0 &&
            strcmp(current->instrumentId, position->instrumentId) == 0) {
            if (position->sourceVersion < current->sourceVersion)
                return UMI_STATUS_INVALID_STATE;
            *current = *position;
            book->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    if (book->count >= UMI_FINANCIAL_POSITION_CONTROL_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    book->positions[book->count++] = *position;
    book->revision += 1U;
    return UMI_STATUS_OK;
}

double umi_financial_position_control_book_market_value(
    const UmiFinancialPositionControlBook *book,
    const char *accountId)
{
    size_t i;
    double total = 0.0;
    if (book == NULL || accountId == NULL) return 0.0;
    for (i = 0U; i < book->count; ++i)
        if (strcmp(book->positions[i].accountId, accountId) == 0)
            total += book->positions[i].quantity * book->positions[i].marketPrice;
    return total;
}

#endif

UmiStatus umi_financial_position_control_validate(const UmiFinancialPositionControl *row)
{
    static const UmiSnapshotTextField fields[] = {
        {"accountId", offsetof(UmiFinancialPositionControl, accountId), sizeof(((UmiFinancialPositionControl *)0)->accountId), 1},
        {"instrumentId", offsetof(UmiFinancialPositionControl, instrumentId), sizeof(((UmiFinancialPositionControl *)0)->instrumentId), 1},
        {"currency", offsetof(UmiFinancialPositionControl, currency), sizeof(((UmiFinancialPositionControl *)0)->currency), 1}
    };
    if (row == NULL || UmiSnapshotValidateTextFields(row, sizeof(*row), fields,
            sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK ||
        !isfinite(row->quantity) ||
        !isfinite(row->averagePrice) ||
        !isfinite(row->marketPrice) ||
        !isfinite(row->realisedPnl)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_financial_position_control_book_validate(const UmiFinancialPositionControlBook *owner)
{
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->count > UMI_FINANCIAL_POSITION_CONTROL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (owner->revision == 0U) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < owner->count; ++i) {
        const UmiFinancialPositionControl *row = &owner->positions[i];
        if (umi_financial_position_control_validate(row) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j) {
            const UmiFinancialPositionControl *previous = &owner->positions[j];
            if (strcmp(row->accountId, previous->accountId) == 0 && strcmp(row->instrumentId, previous->instrumentId) == 0) return UMI_STATUS_INVALID_STATE;
        }
    }
    return UMI_STATUS_OK;
}
void umi_financial_position_control_book_init(UmiFinancialPositionControlBook *owner)
{
    if (owner == NULL) return;
    memset(owner, 0, sizeof(*owner));
    owner->revision = 1U;
}
const UmiFinancialPositionControl *umi_financial_position_control_book_find(const UmiFinancialPositionControlBook *owner,
    const char *accountId, const char *instrumentId)
{
    char accountIdKey[64];
    if (FinancialControlQuery(accountId, accountIdKey, sizeof accountIdKey) != UMI_STATUS_OK) return NULL;
    char instrumentIdKey[64];
    if (FinancialControlQuery(instrumentId, instrumentIdKey, sizeof instrumentIdKey) != UMI_STATUS_OK) return NULL;
    if (umi_financial_position_control_book_validate(owner) != UMI_STATUS_OK) return NULL;
    for (size_t i = 0U; i < owner->count; ++i) {
        const UmiFinancialPositionControl *row = &owner->positions[i];
        if (strcmp(row->accountId, accountIdKey) == 0 && strcmp(row->instrumentId, instrumentIdKey) == 0) return row;
    }
    return NULL;
}
UmiStatus umi_financial_position_control_book_upsert(UmiFinancialPositionControlBook *owner, const UmiFinancialPositionControl *row)
{
    if (owner != NULL && owner->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_financial_position_control_book_validate(owner);
    if (status == UMI_STATUS_OK) status = umi_financial_position_control_validate(row);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < owner->count; ++i) {
        UmiFinancialPositionControl *current = &owner->positions[i];
        if (strcmp(current->accountId, row->accountId) == 0 && strcmp(current->instrumentId, row->instrumentId) == 0) {
            if (row->sourceVersion < current->sourceVersion) return UMI_STATUS_INVALID_STATE;
            *current = *row;
            ++owner->revision;
            return UMI_STATUS_OK;
        }
    }
    if (owner->count == UMI_FINANCIAL_POSITION_CONTROL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    owner->positions[owner->count++] = *row;
    ++owner->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_financial_position_control_book_market_value_checked(
    const UmiFinancialPositionControlBook *book, const char *accountId, double *outValue)
{
    if (outValue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    char key[64];
    UmiStatus status = FinancialControlQuery(accountId, key, sizeof key);
    if (status == UMI_STATUS_OK) status = umi_financial_position_control_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    double total = 0.0;
    const char *currency = NULL;
    for (size_t i = 0U; i < book->count; ++i) {
        const UmiFinancialPositionControl *row = &book->positions[i];
        if (strcmp(row->accountId, key) != 0) continue;
        if (currency == NULL) currency = row->currency;
        else if (strcmp(currency, row->currency) != 0) return UMI_STATUS_INVALID_STATE;
        double value = row->quantity * row->marketPrice;
        if (!isfinite(value)) return UMI_STATUS_CAPACITY_EXCEEDED;
        total += value;
        if (!isfinite(total)) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    *outValue = total;
    return UMI_STATUS_OK;
}
double umi_financial_position_control_book_market_value(
    const UmiFinancialPositionControlBook *book, const char *accountId)
{
    double value;
    return umi_financial_position_control_book_market_value_checked(book, accountId, &value) == UMI_STATUS_OK ? value : 0.0;
}
