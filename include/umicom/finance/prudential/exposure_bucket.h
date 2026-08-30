/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/exposure_bucket.h
 *
 * PURPOSE:
 *   Aggregate exposure amounts for one prudential class.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_EXPOSURE_BUCKET_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_EXPOSURE_BUCKET_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialExposureBucket { char bucket_id[UMI_PRU_ID_CAPACITY]; double amount; UmiCurrency currency; } UmiPrudentialExposureBucket;
/* Initialise one non-negative prudential amount bucket. */
UmiStatus umi_pru_exposure_bucket_init(UmiPrudentialExposureBucket *bucket, const char *bucket_id, double amount, UmiCurrency currency);
/* Add a non-negative amount to the prudential bucket. */
UmiStatus umi_pru_exposure_bucket_add(UmiPrudentialExposureBucket *bucket, double amount);

#ifdef __cplusplus
}
#endif

#endif
