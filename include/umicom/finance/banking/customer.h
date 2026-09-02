/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/customer.h
 *
 * PURPOSE:
 *   Represent a canonical banking customer linked to the shared legal-entity model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_CUSTOMER_H
#define UMICOM_FINANCE_BANKING_CUSTOMER_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking customer data shared with callers of this public contract.
 */
typedef struct UmiBankingCustomer {
    UmiFinancialId id;
    UmiFinancialId legal_entity_id;
    char display_name[UMI_BANKING_NAME_CAPACITY];
    UmiBankingCustomerStatus status;
} UmiBankingCustomer;
/**
 * Initialise banking customer from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_banking_customer_init(UmiBankingCustomer *value,
    const char *id,
    const char *legal_entity_id,
    const char *display_name,
    UmiBankingCustomerStatus status);
/**
 * Check that banking customer satisfies its contract before another service relies on it.
 */
bool umi_banking_customer_valid(const UmiBankingCustomer *value);
/**
 * Provide the banking customer operational operation used by this module and its client
 * applications.
 */
bool umi_banking_customer_operational(const UmiBankingCustomer *value);
#ifdef __cplusplus
}
#endif
#endif
