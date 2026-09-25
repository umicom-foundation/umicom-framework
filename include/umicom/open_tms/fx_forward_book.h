/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/fx_forward_book.h
 *
 * PURPOSE:
 *   Retain bounded FX-forward lifecycle and valuation-control records for Open TMS.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_FX_FORWARD_BOOK_H
#define UMICOM_OPEN_TMS_FX_FORWARD_BOOK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_OPEN_TMS_FX_FORWARD_CAPACITY 512U

typedef enum UmiOpenTmsFxForwardState {
    UMI_OPEN_TMS_FX_FORWARD_DRAFT = 0,
    UMI_OPEN_TMS_FX_FORWARD_CONFIRMED = 1,
    UMI_OPEN_TMS_FX_FORWARD_SETTLEMENT_PENDING = 2,
    UMI_OPEN_TMS_FX_FORWARD_SETTLED = 3,
    UMI_OPEN_TMS_FX_FORWARD_CANCELLED = 4
} UmiOpenTmsFxForwardState;

typedef struct UmiOpenTmsFxForward {
    char tradeId[64];
    char accountId[64];
    char counterpartyId[64];
    char baseCurrency[8];
    char quoteCurrency[8];
    double baseNotional;
    double forwardRate;
    double marketForwardRate;
    uint64_t maturityMilliseconds;
    UmiOpenTmsFxForwardState state;
    uint64_t version;
} UmiOpenTmsFxForward;

typedef struct UmiOpenTmsFxForwardBook {
    UmiOpenTmsFxForward trades[UMI_OPEN_TMS_FX_FORWARD_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiOpenTmsFxForwardBook;

void umi_open_tms_fx_forward_book_init(UmiOpenTmsFxForwardBook *book);
UmiStatus umi_open_tms_fx_forward_book_add(
    UmiOpenTmsFxForwardBook *book,
    const UmiOpenTmsFxForward *trade);
const UmiOpenTmsFxForward *umi_open_tms_fx_forward_book_find(
    const UmiOpenTmsFxForwardBook *book,
    const char *tradeId);
UmiStatus umi_open_tms_fx_forward_book_transition(
    UmiOpenTmsFxForwardBook *book,
    const char *tradeId,
    UmiOpenTmsFxForwardState state,
    uint64_t version);
double umi_open_tms_fx_forward_mark_to_market(
    const UmiOpenTmsFxForward *trade);

#ifdef __cplusplus
}
#endif
#endif
