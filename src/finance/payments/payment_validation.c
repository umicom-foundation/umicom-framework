/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_validation.c
 *
 * PURPOSE:
 *   Implement record payment control-gate evidence before release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_validation.h"
#include <string.h>
/*
 * Initialise payments payment validation from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_validation_init(UmiPaymentsPaymentValidation *value,
    const char *id,
    const char *payment_id,
    uint32_t checks_passed,
    uint32_t checks_required,
    bool sanctions_clear,
    bool sufficient_funds) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_id_assign(&value->payment_id,payment_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->checks_passed=checks_passed;
    value->checks_required=checks_required;
    value->sanctions_clear=sanctions_clear;
    value->sufficient_funds=sufficient_funds;
    return umi_payments_payment_validation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment validation satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payment_validation_valid(const UmiPaymentsPaymentValidation *value) {
    return value!=NULL && (value->checks_required>0U && value->checks_passed<=value->checks_required);
}

/*
 * Provide the payments payment validation passed operation used by this module and its
 * client applications.
 */
bool umi_payments_payment_validation_passed(const UmiPaymentsPaymentValidation *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->checks_passed==value->checks_required && value->sanctions_clear && value->sufficient_funds;
}
