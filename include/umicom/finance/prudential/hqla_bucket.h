/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/hqla_bucket.h
 *
 * PURPOSE:
 *   Aggregate HQLA values by regulatory liquidity level.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_HQLA_BUCKET_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_HQLA_BUCKET_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential hqla bucket data shared with callers of this public contract.
 */
typedef struct UmiPrudentialHqlaBucket { UmiHqlaLevel level; double gross_value; double eligible_value; } UmiPrudentialHqlaBucket;
/* Add gross and eligible value to one HQLA regulatory level bucket. */
UmiStatus umi_pru_hqla_bucket_add(UmiPrudentialHqlaBucket *bucket, UmiHqlaLevel level, double gross_value, double eligible_value);

#ifdef __cplusplus
}
#endif

#endif
