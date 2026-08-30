/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/stable_funding_item.h
 *
 * PURPOSE:
 *   Calculate available stable funding from balance and ASF factor.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STABLE_FUNDING_ITEM_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STABLE_FUNDING_ITEM_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialStableFundingItem { char item_id[UMI_PRU_ID_CAPACITY]; double base_amount; double factor; double stressed_amount; } UmiPrudentialStableFundingItem;
/* Calculate a stressed liquidity or stable-funding amount from a bounded factor. */
UmiStatus umi_pru_stable_funding_item_calculate(UmiPrudentialStableFundingItem *result, const char *item_id, double base_amount, double factor);

#ifdef __cplusplus
}
#endif

#endif
