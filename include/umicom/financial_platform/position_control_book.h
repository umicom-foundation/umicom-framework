/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/position_control_book.h
 *
 * PURPOSE:
 *   Aggregate bounded position-control snapshots by account, instrument and
 *   currency without replacing the canonical finance position engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_POSITION_CONTROL_BOOK_H
#define UMICOM_FINANCIAL_PLATFORM_POSITION_CONTROL_BOOK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FINANCIAL_POSITION_CONTROL_CAPACITY 512U

typedef struct UmiFinancialPositionControl {
    char accountId[64];
    char instrumentId[64];
    char currency[8];
    double quantity;
    double averagePrice;
    double marketPrice;
    double realisedPnl;
    uint64_t sourceVersion;
} UmiFinancialPositionControl;

typedef struct UmiFinancialPositionControlBook {
    UmiFinancialPositionControl positions[UMI_FINANCIAL_POSITION_CONTROL_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiFinancialPositionControlBook;

void umi_financial_position_control_book_init(UmiFinancialPositionControlBook *book);
UmiStatus umi_financial_position_control_book_upsert(
    UmiFinancialPositionControlBook *book,
    const UmiFinancialPositionControl *position);
const UmiFinancialPositionControl *umi_financial_position_control_book_find(
    const UmiFinancialPositionControlBook *book,
    const char *accountId,
    const char *instrumentId);
double umi_financial_position_control_book_market_value(
    const UmiFinancialPositionControlBook *book,
    const char *accountId);

#ifdef __cplusplus
}
#endif
#endif
