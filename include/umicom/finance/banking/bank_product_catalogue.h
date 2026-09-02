/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/bank_product_catalogue.h
 *
 * PURPOSE:
 *   Catalogue reusable banking product templates with stable lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_BANK_PRODUCT_CATALOGUE_H
#define UMICOM_FINANCE_BANKING_BANK_PRODUCT_CATALOGUE_H
#include "umicom/finance/banking/bank_product.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking bank product catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiBankingBankProductCatalogue { UmiBankingBankProduct items[UMI_BANKING_MAX_ITEMS]; size_t count; } UmiBankingBankProductCatalogue;
/**
 * Initialise banking bank product catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_banking_bank_product_catalogue_init(UmiBankingBankProductCatalogue *registry);
/**
 * Add banking bank product catalogue only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_banking_bank_product_catalogue_add(UmiBankingBankProductCatalogue *registry,const UmiBankingBankProduct *item);
/**
 * Find banking bank product catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiBankingBankProduct *umi_banking_bank_product_catalogue_find(const UmiBankingBankProductCatalogue *registry,const char *id);
#ifdef __cplusplus
}
#endif
#endif
