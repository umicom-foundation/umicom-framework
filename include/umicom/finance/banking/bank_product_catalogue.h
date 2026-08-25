/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/bank_product_catalogue.h
 *
 * PURPOSE:
 *   Catalogue reusable banking product templates with stable lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_BANK_PRODUCT_CATALOGUE_H
#define UMICOM_FINANCE_BANKING_BANK_PRODUCT_CATALOGUE_H
#include "umicom/finance/banking/bank_product.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBankingBankProductCatalogue { UmiBankingBankProduct items[UMI_BANKING_MAX_ITEMS]; size_t count; } UmiBankingBankProductCatalogue;
void umi_banking_bank_product_catalogue_init(UmiBankingBankProductCatalogue *registry);
UmiStatus umi_banking_bank_product_catalogue_add(UmiBankingBankProductCatalogue *registry,const UmiBankingBankProduct *item);
const UmiBankingBankProduct *umi_banking_bank_product_catalogue_find(const UmiBankingBankProductCatalogue *registry,const char *id);
#ifdef __cplusplus
}
#endif
#endif
