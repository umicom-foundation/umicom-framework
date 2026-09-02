/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/business_time.h
 *
 * PURPOSE:
 *   Compare business timestamps without binding callers to an operating-system clock API.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module keeps business time logic in one reusable Framework boundary so financial applications do not copy the same rules.
 */

#ifndef INCLUDE_UMICOM_FINANCE_BUSINESS_TIME_H
#define INCLUDE_UMICOM_FINANCE_BUSINESS_TIME_H
#include "umicom/base/status.h"
#include "umicom/finance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the business time compare operation used by this module and its client
 * applications.
 */
int umi_business_time_compare(UmiBusinessTime left, UmiBusinessTime right);
/**
 * Provide the business time elapsed ms operation used by this module and its client
 * applications.
 */
int64_t umi_business_time_elapsed_ms(UmiBusinessTime start, UmiBusinessTime end);
#ifdef __cplusplus
}
#endif
#endif
