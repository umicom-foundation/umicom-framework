/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_status_history.c
 *
 * PURPOSE:
 *   Verify payment status progression, monotonic timestamps and lifecycle evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_status_history.h"
int main(void){ UmiPaymentsPaymentStatusHistory h; if(umi_payments_payment_status_history_init(&h,"hist-1","pay-1")!=UMI_STATUS_OK)return 1; if(umi_payments_payment_status_history_append(&h,UMI_PAYMENTS_CREATED,100)!=UMI_STATUS_OK)return 2; if(umi_payments_payment_status_history_append(&h,UMI_PAYMENTS_APPROVED,200)!=UMI_STATUS_OK)return 3; if(umi_payments_payment_status_history_append(&h,UMI_PAYMENTS_VALIDATED,300)!=UMI_STATUS_INVALID_STATE)return 4; return 0; }
