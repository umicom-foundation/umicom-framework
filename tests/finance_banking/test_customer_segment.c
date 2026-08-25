/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_customer_segment.c
 *
 * PURPOSE:
 *   Exercise customer segment validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/customer_segment.h"
int main(void) {
    UmiBankingCustomerSegment v;
    if(umi_banking_customer_segment_init(&v, "seg-1", "cust-1", UMI_BANKING_SEGMENT_CORPORATE)!=UMI_STATUS_OK) return 1;
    if(!umi_banking_customer_segment_valid(&v)) return 2;
    return 0;
}
