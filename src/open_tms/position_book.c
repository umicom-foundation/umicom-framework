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
#include "umicom/open_tms/position_book.h"
#include <math.h>
#include <string.h>

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
