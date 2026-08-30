/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/prudential_limit_result.h
 *
 * PURPOSE:
 *   Evaluate a prudential metric against a configured control limit.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_PRUDENTIAL_LIMIT_RESULT_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_PRUDENTIAL_LIMIT_RESULT_H

#include "umicom/finance/prudential/types.h"
#include "umicom/finance/prudential/prudential_limit.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialLimitResult { char limit_id[UMI_PRU_ID_CAPACITY]; double observed; double threshold; double headroom; int breached; } UmiPrudentialLimitResult;
/* Evaluate an observed metric against a prudential minimum or maximum. */
UmiStatus umi_pru_prudential_limit_result_evaluate(UmiPrudentialLimitResult *result, const UmiPrudentialLimit *limit, double observed);

#ifdef __cplusplus
}
#endif

#endif
