/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/accounting_service.c
 *
 * PURPOSE:
 *   Implement represent Framework accounting-service readiness and ledger control state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/accounting_service.h"
#include <string.h>
/*
 * Initialise accounting accounting service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_accounting_service_init(UmiAccountingAccountingService *value,
    const char *id,
    bool running,
    size_t account_count,
    size_t posted_journals,
    size_t open_periods,
    size_t control_breaches) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->running=running;
    value->account_count=account_count;
    value->posted_journals=posted_journals;
    value->open_periods=open_periods;
    value->control_breaches=control_breaches;
    return umi_accounting_accounting_service_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting accounting service satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_accounting_service_valid(const UmiAccountingAccountingService *value) {
    return value!=NULL && (true);
}

/*
 * Provide the accounting accounting service ready operation used by this module and its
 * client applications.
 */
bool umi_accounting_accounting_service_ready(const UmiAccountingAccountingService *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->running && value->control_breaches==0U;
}
