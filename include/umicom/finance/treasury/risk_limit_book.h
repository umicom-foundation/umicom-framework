/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/risk_limit_book.h
 *
 * PURPOSE:
 *   Maintain reusable treasury risk limits with duplicate prevention.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_RISK_LIMIT_BOOK_H
#define UMICOM_FINANCE_TREASURY_RISK_LIMIT_BOOK_H
#include "umicom/finance/treasury/risk_limit.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryRiskLimitBook {
    UmiTreasuryRiskLimit items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasuryRiskLimitBook;
void umi_treasury_risk_limit_book_init(UmiTreasuryRiskLimitBook *registry);
UmiStatus umi_treasury_risk_limit_book_add(UmiTreasuryRiskLimitBook *registry, const UmiTreasuryRiskLimit *item);
const UmiTreasuryRiskLimit *umi_treasury_risk_limit_book_find(const UmiTreasuryRiskLimitBook *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
