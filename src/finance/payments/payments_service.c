/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payments_service.c
 *
 * PURPOSE:
 *   Implement represent Framework payment-service readiness, volume and exception evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payments_service.h"
#include <string.h>
/*
 * Initialise payments payments service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payments_service_init(UmiPaymentsPaymentsService *value,
    const char *id,
    bool running,
    size_t instruction_count,
    size_t settled_count,
    size_t exception_count,
    size_t unmatched_count) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->running=running;
    value->instruction_count=instruction_count;
    value->settled_count=settled_count;
    value->exception_count=exception_count;
    value->unmatched_count=unmatched_count;
    return umi_payments_payments_service_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payments service satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payments_service_valid(const UmiPaymentsPaymentsService *value) {
    return value!=NULL && (value->settled_count<=value->instruction_count);
}

/*
 * Provide the payments payments service ready operation used by this module and its client
 * applications.
 */
bool umi_payments_payments_service_ready(const UmiPaymentsPaymentsService *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->running && value->exception_count==0U && value->unmatched_count==0U;
}
