/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_customer_segment.c
 *
 * PURPOSE:
 *   Exercise customer segment validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/customer_segment.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiBankingCustomerSegment v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_customer_segment_init(&v, "seg-1", "cust-1", UMI_BANKING_SEGMENT_CORPORATE)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_banking_customer_segment_valid(&v)) return 2;
    return 0;
}
