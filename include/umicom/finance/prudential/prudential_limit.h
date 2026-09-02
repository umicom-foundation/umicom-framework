/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/prudential_limit.h
 *
 * PURPOSE:
 *   Define a prudential minimum or maximum control limit.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_PRUDENTIAL_LIMIT_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_PRUDENTIAL_LIMIT_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential limit data shared with callers of this public contract.
 */
typedef struct UmiPrudentialLimit { char limit_id[UMI_PRU_ID_CAPACITY]; double threshold; int is_minimum; UmiPrudentialSeverity severity; } UmiPrudentialLimit;
/* Initialise a prudential minimum or maximum control limit. */
UmiStatus umi_pru_prudential_limit_init(UmiPrudentialLimit *limit, const char *limit_id, double threshold, int is_minimum, UmiPrudentialSeverity severity);

#ifdef __cplusplus
}
#endif

#endif
