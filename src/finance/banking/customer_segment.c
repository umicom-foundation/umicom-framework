/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/customer_segment.c
 *
 * PURPOSE:
 *   Implement assign a reusable banking customer segment for product and service policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/customer_segment.h"
#include <string.h>
/*
 * Initialise banking customer segment from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_banking_customer_segment_init(UmiBankingCustomerSegment *value,
    const char *id,
    const char *customer_id,
    UmiBankingSegment segment) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->customer_id,customer_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->segment=segment;
    return umi_banking_customer_segment_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that banking customer segment satisfies its contract before another service relies
 * on it.
 */
bool umi_banking_customer_segment_valid(const UmiBankingCustomerSegment *value) {
    return value!=NULL && (umi_financial_id_is_valid(&value->customer_id) && value->segment>=UMI_BANKING_SEGMENT_RETAIL && value->segment<=UMI_BANKING_SEGMENT_INSTITUTIONAL);
}

/*
 * Provide the banking customer segment institutional operation used by this module and its
 * client applications.
 */
bool umi_banking_customer_segment_institutional(const UmiBankingCustomerSegment *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->segment==UMI_BANKING_SEGMENT_INSTITUTIONAL;
}
