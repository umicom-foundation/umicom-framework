/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/risk_book.h
 *
 * PURPOSE:
 *   Retain treasury risk limits and utilisation/breach evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_RISK_BOOK_H
#define UMICOM_OPEN_TMS_RISK_BOOK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_OPEN_TMS_RISK_LIMIT_CAPACITY 256U

typedef struct UmiOpenTmsRiskLimit {
    char id[64];
    char scopeId[64];
    double limit;
    double utilisation;
    uint64_t version;
    int hardLimit;
} UmiOpenTmsRiskLimit;

typedef struct UmiOpenTmsRiskBook {
    UmiOpenTmsRiskLimit limits[UMI_OPEN_TMS_RISK_LIMIT_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiOpenTmsRiskBook;

void umi_open_tms_risk_book_init(UmiOpenTmsRiskBook *book);
UmiStatus umi_open_tms_risk_book_upsert(
    UmiOpenTmsRiskBook *book,
    const UmiOpenTmsRiskLimit *limit);
const UmiOpenTmsRiskLimit *umi_open_tms_risk_book_find(
    const UmiOpenTmsRiskBook *book,
    const char *id);
size_t umi_open_tms_risk_book_breach_count(const UmiOpenTmsRiskBook *book);
double umi_open_tms_risk_limit_utilisation_percent(const UmiOpenTmsRiskLimit *limit);

#ifdef __cplusplus
}
#endif
#endif
