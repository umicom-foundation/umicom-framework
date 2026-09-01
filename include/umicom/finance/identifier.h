/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/identifier.h
 *
 * PURPOSE:
 *   Validate and compare stable financial identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module keeps identifier logic in one reusable Framework boundary so financial applications do not copy the same rules.
 */

#ifndef INCLUDE_UMICOM_FINANCE_IDENTIFIER_H
#define INCLUDE_UMICOM_FINANCE_IDENTIFIER_H
#include "umicom/base/status.h"
#include "umicom/finance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_financial_id_valid(const UmiFinancialId *id);
int umi_financial_id_equal(const UmiFinancialId *left, const UmiFinancialId *right);
#ifdef __cplusplus
}
#endif
#endif
