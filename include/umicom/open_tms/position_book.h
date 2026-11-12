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

/* Admit complete bounded public storage, unique account/instrument keys and
 * finite observations before traversal. Inline text is nonempty and terminated;
 * currency is a label, not ISO membership. Short positions and signed prices
 * remain permitted. Keep values stable on their owning thread. */
UmiStatus umi_open_tms_position_validate(const UmiOpenTmsPosition *position);
UmiStatus umi_open_tms_position_book_validate(const UmiOpenTmsPositionBook *book);
typedef struct UmiOpenTmsPositionMeasures {
    double grossExposure; double netExposure; double unrealisedPnl;
} UmiOpenTmsPositionMeasures;
/* One currency label only: no FX conversion is performed. Empty valid storage
 * gives zero. All intermediate products/sums must be finite; otherwise out is
 * unchanged. Output must be disjoint from input. Scalar wrappers delegate to
 * this complete calculation and return zero on refusal; use status for reports. */
UmiStatus umi_open_tms_position_book_measure(const UmiOpenTmsPositionBook *book, UmiOpenTmsPositionMeasures *out);

void umi_open_tms_position_book_init(UmiOpenTmsPositionBook *book);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
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
