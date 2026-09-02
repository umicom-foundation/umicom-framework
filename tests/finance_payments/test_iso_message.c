/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_iso_message.c
 *
 * PURPOSE:
 *   Exercise iso message validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/iso_message.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsIsoMessage v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_iso_message_init(&v, "iso-1", "pay-1", "pacs.008", "E2E-1")!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_iso_message_valid(&v)) return 2;
    return 0;
}
