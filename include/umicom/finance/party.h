/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/party.h
 *
 * PURPOSE:
 *   Validate canonical party records used by broker, TMS and banking adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module keeps party logic in one reusable Framework boundary so financial applications do not copy the same rules.
 */

#ifndef INCLUDE_UMICOM_FINANCE_PARTY_H
#define INCLUDE_UMICOM_FINANCE_PARTY_H
#include "umicom/base/status.h"
#include "umicom/finance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Check that financial party satisfies its contract before another service relies on it.
 */
int umi_financial_party_valid(const UmiFinancialParty *party);
#ifdef __cplusplus
}
#endif
#endif
