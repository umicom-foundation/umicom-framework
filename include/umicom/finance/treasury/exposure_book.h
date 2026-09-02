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
/**
 * Represent the treasury exposure book data shared with callers of this public contract.
 */
typedef struct UmiTreasuryExposureBook {
    UmiTreasuryExposure items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasuryExposureBook;
/**
 * Initialise treasury exposure book from caller-provided values so later operations
 * receive a known state.
 */
void umi_treasury_exposure_book_init(UmiTreasuryExposureBook *registry);
/**
 * Add treasury exposure book only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_treasury_exposure_book_add(UmiTreasuryExposureBook *registry, const UmiTreasuryExposure *item);
/**
 * Find treasury exposure book while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiTreasuryExposure *umi_treasury_exposure_book_find(const UmiTreasuryExposureBook *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
