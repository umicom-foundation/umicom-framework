/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/customer_segment.c
 *
 * PURPOSE:
 *   Implement assign a reusable banking customer segment for product and service policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/customer_segment.h"
#include <string.h>
UmiStatus umi_banking_customer_segment_init(UmiBankingCustomerSegment *value,
    const char *id,
    const char *customer_id,
    UmiBankingSegment segment) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->customer_id,customer_id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->segment=segment;
    return umi_banking_customer_segment_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_banking_customer_segment_valid(const UmiBankingCustomerSegment *value) {
    return value!=NULL && (umi_financial_id_is_valid(&value->customer_id) && value->segment>=UMI_BANKING_SEGMENT_RETAIL && value->segment<=UMI_BANKING_SEGMENT_INSTITUTIONAL);
}

bool umi_banking_customer_segment_institutional(const UmiBankingCustomerSegment *value) {
    if(value==NULL) return (bool)0;
    return value->segment==UMI_BANKING_SEGMENT_INSTITUTIONAL;
}
