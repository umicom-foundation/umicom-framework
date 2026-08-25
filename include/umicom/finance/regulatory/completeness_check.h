/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/completeness_check.h
 *
 * PURPOSE:
 *   Compute regulatory record completeness from required and populated fields.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_COMPLETENESS_CHECK_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_COMPLETENESS_CHECK_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Compute populated/required completeness, treating no requirements as complete. */
UmiStatus umi_reg_completeness_check(size_t required,size_t populated,double *out_score);

#ifdef __cplusplus
}
#endif

#endif
