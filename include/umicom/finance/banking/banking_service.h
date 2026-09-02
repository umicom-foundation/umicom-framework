/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/banking_service.h
 *
 * PURPOSE:
 *   Represent Framework banking-service readiness and aggregate exception state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_BANKING_SERVICE_H
#define UMICOM_FINANCE_BANKING_BANKING_SERVICE_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking banking service data shared with callers of this public contract.
 */
typedef struct UmiBankingBankingService {
    UmiFinancialId id;
    bool running;
    size_t active_customers;
    size_t open_accounts;
    size_t open_loans;
    size_t control_exceptions;
} UmiBankingBankingService;
/**
 * Initialise banking banking service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_banking_banking_service_init(UmiBankingBankingService *value,
    const char *id,
    bool running,
    size_t active_customers,
    size_t open_accounts,
    size_t open_loans,
    size_t control_exceptions);
/**
 * Check that banking banking service satisfies its contract before another service relies
 * on it.
 */
bool umi_banking_banking_service_valid(const UmiBankingBankingService *value);
/**
 * Provide the banking banking service ready operation used by this module and its client
 * applications.
 */
bool umi_banking_banking_service_ready(const UmiBankingBankingService *value);
#ifdef __cplusplus
}
#endif
#endif
