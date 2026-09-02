/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/banking_service.c
 *
 * PURPOSE:
 *   Implement represent Framework banking-service readiness and aggregate exception state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/banking_service.h"
#include <string.h>
/*
 * Initialise banking banking service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_banking_banking_service_init(UmiBankingBankingService *value,
    const char *id,
    bool running,
    size_t active_customers,
    size_t open_accounts,
    size_t open_loans,
    size_t control_exceptions) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->running=running;
    value->active_customers=active_customers;
    value->open_accounts=open_accounts;
    value->open_loans=open_loans;
    value->control_exceptions=control_exceptions;
    return umi_banking_banking_service_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that banking banking service satisfies its contract before another service relies
 * on it.
 */
bool umi_banking_banking_service_valid(const UmiBankingBankingService *value) {
    return value!=NULL && (true);
}

/*
 * Provide the banking banking service ready operation used by this module and its client
 * applications.
 */
bool umi_banking_banking_service_ready(const UmiBankingBankingService *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->running && value->control_exceptions==0U;
}
