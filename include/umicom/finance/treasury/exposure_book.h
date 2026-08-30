/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/exposure_book.h
 *
 * PURPOSE:
 *   Maintain bounded exposure records for aggregation and reporting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_EXPOSURE_BOOK_H
#define UMICOM_FINANCE_TREASURY_EXPOSURE_BOOK_H
#include "umicom/finance/treasury/exposure.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryExposureBook {
    UmiTreasuryExposure items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasuryExposureBook;
void umi_treasury_exposure_book_init(UmiTreasuryExposureBook *registry);
UmiStatus umi_treasury_exposure_book_add(UmiTreasuryExposureBook *registry, const UmiTreasuryExposure *item);
const UmiTreasuryExposure *umi_treasury_exposure_book_find(const UmiTreasuryExposureBook *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
