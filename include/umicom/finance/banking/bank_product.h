/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/bank_product.h
 *
 * PURPOSE:
 *   Describe reusable banking product templates independent of channel applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_BANK_PRODUCT_H
#define UMICOM_FINANCE_BANKING_BANK_PRODUCT_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking bank product data shared with callers of this public contract.
 */
typedef struct UmiBankingBankProduct {
    UmiFinancialId id;
    char name[UMI_BANKING_NAME_CAPACITY];
    UmiBankingProductKind kind;
    bool active;
} UmiBankingBankProduct;
/**
 * Initialise banking bank product from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_banking_bank_product_init(UmiBankingBankProduct *value,
    const char *id,
    const char *name,
    UmiBankingProductKind kind,
    bool active);
/**
 * Check that banking bank product satisfies its contract before another service relies on
 * it.
 */
bool umi_banking_bank_product_valid(const UmiBankingBankProduct *value);
/**
 * Provide the banking bank product available operation used by this module and its client
 * applications.
 */
bool umi_banking_bank_product_available(const UmiBankingBankProduct *value);
#ifdef __cplusplus
}
#endif
#endif
