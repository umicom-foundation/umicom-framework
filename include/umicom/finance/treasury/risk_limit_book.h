/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/risk_limit_book.h
 *
 * PURPOSE:
 *   Maintain reusable treasury risk limits with duplicate prevention.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_RISK_LIMIT_BOOK_H
#define UMICOM_FINANCE_TREASURY_RISK_LIMIT_BOOK_H
#include "umicom/finance/treasury/risk_limit.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury risk limit book data shared with callers of this public contract.
 */
typedef struct UmiTreasuryRiskLimitBook {
    UmiTreasuryRiskLimit items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasuryRiskLimitBook;
/**
 * Initialise treasury risk limit book from caller-provided values so later operations
 * receive a known state.
 */
void umi_treasury_risk_limit_book_init(UmiTreasuryRiskLimitBook *registry);
/**
 * Add treasury risk limit book only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_treasury_risk_limit_book_add(UmiTreasuryRiskLimitBook *registry, const UmiTreasuryRiskLimit *item);
/**
 * Find treasury risk limit book while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiTreasuryRiskLimit *umi_treasury_risk_limit_book_find(const UmiTreasuryRiskLimitBook *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
