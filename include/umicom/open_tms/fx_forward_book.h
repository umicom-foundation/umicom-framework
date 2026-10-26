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

/* This bounded owner stores observations, not authenticated trade permissions.
 * Added records may carry any known state for imported evidence. Identity fields
 * must terminate within their arrays and contain nonblank, control-free bytes.
 * Currency codes are distinct three-letter uppercase codes. Notional/rates must
 * be finite; notional and contractual rate are positive. A zero market rate means
 * unavailable. Invalid rows never enter the book; corrupt public book contents
 * are refused rather than traversed beyond capacity.
 *
 * Transition is strictly Draft -> Confirmed -> Settlement pending -> Settled.
 * Cancellation is allowed from the first three states. Same-state, backwards,
 * skipped-stage, unknown-state and terminal transitions are refused. The supplied
 * trade version must increase; failures leave the complete book unchanged.
 * Maturity zero remains an unspecified timestamp. This API does not check clocks,
 * approvals, cash or external settlement; use a reviewed application service for
 * those separate responsibilities. Serialise all access and initialise first.
 * Find borrows a pointer until the next mutation; invalid books return NULL. */
void umi_open_tms_fx_forward_book_init(UmiOpenTmsFxForwardBook *book);
/* Check the complete inline record set without mutation or execution. NULL
 * returns INVALID_ARGUMENT; malformed active state returns INVALID_STATE. */
UmiStatus umi_open_tms_fx_forward_book_validate(const UmiOpenTmsFxForwardBook *book);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_open_tms_fx_forward_book_add(
    UmiOpenTmsFxForwardBook *book,
    const UmiOpenTmsFxForward *trade);
const UmiOpenTmsFxForward *umi_open_tms_fx_forward_book_find(
    const UmiOpenTmsFxForwardBook *book,
    const char *tradeId);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_open_tms_fx_forward_book_transition(
    UmiOpenTmsFxForwardBook *book,
    const char *tradeId,
    UmiOpenTmsFxForwardState state,
    uint64_t version);
/* Calculate notional * (market forward rate - contract forward rate).
 * This is an undiscounted observation, not a production pricing model or a
 * posting amount. Only numeric fields participate. Zero market rate returns
 * UNAVAILABLE; non-finite or invalid values return INVALID_ARGUMENT; overflow
 * returns CAPACITY_EXCEEDED. Failure leaves *outValue unchanged.
 * Legacy scalar API below remains available and returns zero on refusal. */
UmiStatus umi_open_tms_fx_forward_mark_to_market_checked(
    const UmiOpenTmsFxForward *trade, double *outValue);

double umi_open_tms_fx_forward_mark_to_market(
    const UmiOpenTmsFxForward *trade);

#ifdef __cplusplus
}
#endif
#endif
