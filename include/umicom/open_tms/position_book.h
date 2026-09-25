/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/position_book.h
 *
 * PURPOSE:
 *   Aggregate treasury currency positions and mark-to-market evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_POSITION_BOOK_H
#define UMICOM_OPEN_TMS_POSITION_BOOK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_OPEN_TMS_POSITION_CAPACITY 512U

typedef struct UmiOpenTmsPosition {
    char accountId[64];
    char instrumentId[64];
    char currency[8];
    double quantity;
    double averagePrice;
    double marketPrice;
    double realisedPnl;
    uint64_t version;
} UmiOpenTmsPosition;

typedef struct UmiOpenTmsPositionBook {
    UmiOpenTmsPosition positions[UMI_OPEN_TMS_POSITION_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiOpenTmsPositionBook;

void umi_open_tms_position_book_init(UmiOpenTmsPositionBook *book);
UmiStatus umi_open_tms_position_book_upsert(
    UmiOpenTmsPositionBook *book,
    const UmiOpenTmsPosition *position);
const UmiOpenTmsPosition *umi_open_tms_position_book_find(
    const UmiOpenTmsPositionBook *book,
    const char *accountId,
    const char *instrumentId);
double umi_open_tms_position_book_gross(const UmiOpenTmsPositionBook *book);
double umi_open_tms_position_book_net(const UmiOpenTmsPositionBook *book);
double umi_open_tms_position_book_unrealised_pnl(const UmiOpenTmsPositionBook *book);

#ifdef __cplusplus
}
#endif
#endif
