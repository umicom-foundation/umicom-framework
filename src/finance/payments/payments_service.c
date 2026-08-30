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
UmiStatus umi_payments_payments_service_init(UmiPaymentsPaymentsService *value,
    const char *id,
    bool running,
    size_t instruction_count,
    size_t settled_count,
    size_t exception_count,
    size_t unmatched_count) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->running=running;
    value->instruction_count=instruction_count;
    value->settled_count=settled_count;
    value->exception_count=exception_count;
    value->unmatched_count=unmatched_count;
    return umi_payments_payments_service_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_payments_payments_service_valid(const UmiPaymentsPaymentsService *value) {
    return value!=NULL && (value->settled_count<=value->instruction_count);
}

bool umi_payments_payments_service_ready(const UmiPaymentsPaymentsService *value) {
    if(value==NULL) return (bool)0;
    return value->running && value->exception_count==0U && value->unmatched_count==0U;
}
