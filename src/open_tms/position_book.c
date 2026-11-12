/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/position_book.c
 *
 * PURPOSE:
 *   Implement monotonic treasury positions and aggregate exposure/P&L evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/open_tms/position_book.h"
#include <math.h>
#include <string.h>

#include "operational_internal.h"

/* Retained for owner review: these implementations trusted public counts,
 * inline strings and unchecked arithmetic. The active Framework boundary below
 * validates complete owners before traversal and commits results only after
 * finite calculations succeed. Existing entry points remain available. */
#if 0
void umi_open_tms_position_book_init(UmiOpenTmsPositionBook *book)
{
    if (book == NULL) return;
    (void)memset(book, 0, sizeof(*book));
    book->revision = 1U;
}

const UmiOpenTmsPosition *umi_open_tms_position_book_find(
    const UmiOpenTmsPositionBook *book,
    const char *accountId,
    const char *instrumentId)
{
    size_t i;
    if (book == NULL || accountId == NULL || instrumentId == NULL) return NULL;
    for (i=0U;i<book->count;++i)
        if (strcmp(book->positions[i].accountId,accountId)==0 &&
            strcmp(book->positions[i].instrumentId,instrumentId)==0)
            return &book->positions[i];
    return NULL;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_open_tms_position_book_upsert(
    UmiOpenTmsPositionBook *book,
    const UmiOpenTmsPosition *position)
{
    size_t i;
    if (book == NULL || position == NULL ||
        position->accountId[0]=='\0' || position->instrumentId[0]=='\0' ||
        position->currency[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<book->count;++i) {
        UmiOpenTmsPosition *current=&book->positions[i];
        if (strcmp(current->accountId,position->accountId)==0 &&
            strcmp(current->instrumentId,position->instrumentId)==0) {
            if (position->version < current->version) return UMI_STATUS_INVALID_STATE;
            *current=*position; book->revision+=1U; return UMI_STATUS_OK;
        }
    }
    if (book->count>=UMI_OPEN_TMS_POSITION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    book->positions[book->count++]=*position; book->revision+=1U; return UMI_STATUS_OK;
}
#endif

UmiStatus umi_open_tms_position_book_upsert(
    UmiOpenTmsPositionBook *book,
    const UmiOpenTmsPosition *position)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (book != NULL && book->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    size_t i;
    if (book == NULL || position == NULL ||
        position->accountId[0]=='\0' || position->instrumentId[0]=='\0' ||
        position->currency[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<book->count;++i) {
        UmiOpenTmsPosition *current=&book->positions[i];
        if (strcmp(current->accountId,position->accountId)==0 &&
            strcmp(current->instrumentId,position->instrumentId)==0) {
            if (position->version < current->version) return UMI_STATUS_INVALID_STATE;
            *current=*position; book->revision+=1U; return UMI_STATUS_OK;
        }
    }
    if (book->count>=UMI_OPEN_TMS_POSITION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    book->positions[book->count++]=*position; book->revision+=1U; return UMI_STATUS_OK;
}

double umi_open_tms_position_book_gross(const UmiOpenTmsPositionBook *book)
{
    size_t i; double total=0.0;
    if (book==NULL) return 0.0;
    for(i=0U;i<book->count;++i)
        total += fabs(book->positions[i].quantity * book->positions[i].marketPrice);
    return total;
}

double umi_open_tms_position_book_net(const UmiOpenTmsPositionBook *book)
{
    size_t i; double total=0.0;
    if (book==NULL) return 0.0;
    for(i=0U;i<book->count;++i)
        total += book->positions[i].quantity * book->positions[i].marketPrice;
    return total;
}

double umi_open_tms_position_book_unrealised_pnl(const UmiOpenTmsPositionBook *book)
{
    size_t i; double total=0.0;
    if (book==NULL) return 0.0;
    for(i=0U;i<book->count;++i)
        total += book->positions[i].quantity *
            (book->positions[i].marketPrice - book->positions[i].averagePrice);
    return total;
}

#endif

UmiStatus umi_open_tms_position_validate(const UmiOpenTmsPosition *position)
{
    static const UmiSnapshotTextField fields[] = {
        {"accountId", offsetof(UmiOpenTmsPosition, accountId), 64U, 1},
        {"instrumentId", offsetof(UmiOpenTmsPosition, instrumentId), 64U, 1},
        {"currency", offsetof(UmiOpenTmsPosition, currency), 8U, 1}
    };
    if (position == NULL || UmiSnapshotValidateTextFields(position, sizeof(*position),
            fields, sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK ||
        !isfinite(position->quantity) || !isfinite(position->averagePrice) ||
        !isfinite(position->marketPrice) || !isfinite(position->realisedPnl)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_open_tms_position_book_validate(const UmiOpenTmsPositionBook *book)
{
    if (book == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (book->count > UMI_OPEN_TMS_POSITION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (book->revision == 0U) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < book->count; ++i) {
        const UmiOpenTmsPosition *row = &book->positions[i];
        if (umi_open_tms_position_validate(row) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(row->accountId, book->positions[j].accountId) == 0 &&
                strcmp(row->instrumentId, book->positions[j].instrumentId) == 0)
                return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
void umi_open_tms_position_book_init(UmiOpenTmsPositionBook *book)
{
    if (book == NULL) return;
    memset(book, 0, sizeof(*book));
    book->revision = 1U;
}
const UmiOpenTmsPosition *umi_open_tms_position_book_find(
    const UmiOpenTmsPositionBook *book, const char *accountId, const char *instrumentId)
{
    if (!TmsOpsQuery(accountId, 64U) || !TmsOpsQuery(instrumentId, 64U) ||
        umi_open_tms_position_book_validate(book) != UMI_STATUS_OK) return NULL;
    for (size_t i = 0U; i < book->count; ++i)
        if (strcmp(book->positions[i].accountId, accountId) == 0 &&
            strcmp(book->positions[i].instrumentId, instrumentId) == 0) return &book->positions[i];
    return NULL;
}
UmiStatus umi_open_tms_position_book_upsert(UmiOpenTmsPositionBook *book,
    const UmiOpenTmsPosition *position)
{
    if (book != NULL && book->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_open_tms_position_book_validate(book);
    if (status == UMI_STATUS_OK) status = umi_open_tms_position_validate(position);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < book->count; ++i) {
        UmiOpenTmsPosition *current = &book->positions[i];
        if (strcmp(current->accountId, position->accountId) == 0 &&
            strcmp(current->instrumentId, position->instrumentId) == 0) {
            if (position->version < current->version) return UMI_STATUS_INVALID_STATE;
            *current = *position;
            ++book->revision;
            return UMI_STATUS_OK;
        }
    }
    if (book->count == UMI_OPEN_TMS_POSITION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    book->positions[book->count++] = *position;
    ++book->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_open_tms_position_book_measure(const UmiOpenTmsPositionBook *book,
    UmiOpenTmsPositionMeasures *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_open_tms_position_book_validate(book);
    if (status != UMI_STATUS_OK) return status;
    UmiOpenTmsPositionMeasures value = {0};
    for (size_t i = 0U; i < book->count; ++i) {
        const UmiOpenTmsPosition *row = &book->positions[i];
        double exposure, spread, pnl;
        if (strcmp(row->currency, book->positions[0].currency) != 0) return UMI_STATUS_INVALID_STATE;
        status = TmsOpsMultiply(row->quantity, row->marketPrice, &exposure);
        if (status == UMI_STATUS_OK) status = TmsOpsAdd(row->marketPrice, -row->averagePrice, &spread);
        if (status == UMI_STATUS_OK) status = TmsOpsMultiply(row->quantity, spread, &pnl);
        if (status == UMI_STATUS_OK) status = TmsOpsAdd(value.grossExposure, fabs(exposure), &value.grossExposure);
        if (status == UMI_STATUS_OK) status = TmsOpsAdd(value.netExposure, exposure, &value.netExposure);
        if (status == UMI_STATUS_OK) status = TmsOpsAdd(value.unrealisedPnl, pnl, &value.unrealisedPnl);
        if (status != UMI_STATUS_OK) return status;
    }
    *out = value;
    return UMI_STATUS_OK;
}
double umi_open_tms_position_book_gross(const UmiOpenTmsPositionBook *book)
{
    UmiOpenTmsPositionMeasures value;
    return umi_open_tms_position_book_measure(book, &value) == UMI_STATUS_OK ? value.grossExposure : 0.0;
}
double umi_open_tms_position_book_net(const UmiOpenTmsPositionBook *book)
{
    UmiOpenTmsPositionMeasures value;
    return umi_open_tms_position_book_measure(book, &value) == UMI_STATUS_OK ? value.netExposure : 0.0;
}
double umi_open_tms_position_book_unrealised_pnl(const UmiOpenTmsPositionBook *book)
{
    UmiOpenTmsPositionMeasures value;
    return umi_open_tms_position_book_measure(book, &value) == UMI_STATUS_OK ? value.unrealisedPnl : 0.0;
}
