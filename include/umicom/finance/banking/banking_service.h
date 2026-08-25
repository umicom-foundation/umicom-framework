/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/banking_service.h
 *
 * PURPOSE:
 *   Represent Framework banking-service readiness and aggregate exception state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_BANKING_SERVICE_H
#define UMICOM_FINANCE_BANKING_BANKING_SERVICE_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBankingBankingService {
    UmiFinancialId id;
    bool running;
    size_t active_customers;
    size_t open_accounts;
    size_t open_loans;
    size_t control_exceptions;
} UmiBankingBankingService;
UmiStatus umi_banking_banking_service_init(UmiBankingBankingService *value,
    const char *id,
    bool running,
    size_t active_customers,
    size_t open_accounts,
    size_t open_loans,
    size_t control_exceptions);
bool umi_banking_banking_service_valid(const UmiBankingBankingService *value);
bool umi_banking_banking_service_ready(const UmiBankingBankingService *value);
#ifdef __cplusplus
}
#endif
#endif
