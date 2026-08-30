/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/timeliness_check.h
 *
 * PURPOSE:
 *   Evaluate source-data timeliness against a maximum regulatory age.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_TIMELINESS_CHECK_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_TIMELINESS_CHECK_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return one when as-of evidence is non-future and within maximum age. */
int umi_reg_timeliness_check(int64_t as_of_ms,int64_t now_ms,int64_t maximum_age_ms);

#ifdef __cplusplus
}
#endif

#endif
