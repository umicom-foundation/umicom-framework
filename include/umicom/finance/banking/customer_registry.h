/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/customer_registry.h
 *
 * PURPOSE:
 *   Maintain bounded canonical banking customers with deterministic identifier lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_CUSTOMER_REGISTRY_H
#define UMICOM_FINANCE_BANKING_CUSTOMER_REGISTRY_H
#include "umicom/finance/banking/customer.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking customer registry data shared with callers of this public
 * contract.
 */
typedef struct UmiBankingCustomerRegistry { UmiBankingCustomer items[UMI_BANKING_MAX_ITEMS]; size_t count; } UmiBankingCustomerRegistry;
/**
 * Initialise banking customer registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_banking_customer_registry_init(UmiBankingCustomerRegistry *registry);
/**
 * Add banking customer registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_banking_customer_registry_add(UmiBankingCustomerRegistry *registry,const UmiBankingCustomer *item);
/**
 * Find banking customer registry while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiBankingCustomer *umi_banking_customer_registry_find(const UmiBankingCustomerRegistry *registry,const char *id);
#ifdef __cplusplus
}
#endif
#endif
