/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/stress_result_store.h
 *
 * PURPOSE:
 *   Store bounded stress results and count failed control thresholds.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_RESULT_STORE_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_RESULT_STORE_H

#include "umicom/finance/prudential/types.h"
#include "umicom/finance/prudential/stress_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential stress result store data shared with callers of this public
 * contract.
 */
typedef struct UmiPrudentialStressResultStore { UmiPrudentialStressResult results[UMI_PRU_MAX_ITEMS]; size_t count; } UmiPrudentialStressResultStore;
/* Append one stress result to the bounded store. */
UmiStatus umi_pru_stress_result_store_add(UmiPrudentialStressResultStore *store, const UmiPrudentialStressResult *result);
/* Count failed stress thresholds in the stored result set. */
size_t umi_pru_stress_result_store_failed_count(const UmiPrudentialStressResultStore *store);

#ifdef __cplusplus
}
#endif

#endif
