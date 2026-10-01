/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_charge.c
 *
 * PURPOSE:
 *   Implement record payment fee and tax components without altering payment principal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_charge.h"
#include <string.h>
#include "umicom/finance/identifier.h"
/*
 * Initialise payments payment charge from caller-provided values so later operations
 * receive a known state.
 */
/* Publish only validated fee and tax evidence; failure retains the previous caller-owned charge, including when input IDs alias it. The previous implementation remains for engineering review. */
#if 0
UmiStatus umi_payments_payment_charge_init(UmiPaymentsPaymentCharge *value,
    const char *id,
    const char *payment_id,
    int64_t fee_minor,
    int64_t tax_minor) {
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
    value->fee_minor=fee_minor;
    value->tax_minor=tax_minor;
    return umi_payments_payment_charge_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
#endif
UmiStatus umi_payments_payment_charge_init(UmiPaymentsPaymentCharge *value,
    const char *id, const char *payment_id, int64_t fee_minor, int64_t tax_minor)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiPaymentsPaymentCharge candidate = {0};
    UmiStatus status = umi_payments_id_assign(&candidate.id, id);
    if (status == UMI_STATUS_OK) status = umi_payments_id_assign(&candidate.payment_id, payment_id);
    if (status != UMI_STATUS_OK) return status;
    candidate.fee_minor = fee_minor; candidate.tax_minor = tax_minor;
    int64_t total;
    status = umi_payments_payment_charge_total_checked(&candidate, &total);
    if (status == UMI_STATUS_OK) *value = candidate;
    return status;
}
/*
 * Check that payments payment charge satisfies its contract before another service relies
 * on it.
 */
/* A usable charge needs bounded identities and a representable total, not only nonnegative components. The previous implementation remains for engineering review. */
#if 0
bool umi_payments_payment_charge_valid(const UmiPaymentsPaymentCharge *value) {
    return value!=NULL && (value->fee_minor>=0 && value->tax_minor>=0);
}
#endif
bool umi_payments_payment_charge_valid(const UmiPaymentsPaymentCharge *value)
{
    int64_t total;
    return umi_payments_payment_charge_total_checked(value, &total) == UMI_STATUS_OK;
}

/*
 * Provide the payments payment charge total minor operation used by this module and its
 * client applications.
 */
/* Keep the scalar API while avoiding undefined signed addition; status-aware consumers use the checked total API. The previous implementation remains for engineering review. */
#if 0
int64_t umi_payments_payment_charge_total_minor(const UmiPaymentsPaymentCharge *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (int64_t)0;
    return value->fee_minor+value->tax_minor;
}
#endif
int64_t umi_payments_payment_charge_total_minor(const UmiPaymentsPaymentCharge *value)
{
    int64_t total = 0;
    (void)umi_payments_payment_charge_total_checked(value, &total);
    return total;
}


UmiStatus umi_payments_payment_charge_total_checked(const UmiPaymentsPaymentCharge *value,
    int64_t *outTotal)
{
    if (outTotal == NULL || value == NULL || !umi_financial_id_valid(&value->id) ||
        !umi_financial_id_valid(&value->payment_id) || value->fee_minor < 0 || value->tax_minor < 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (value->tax_minor > INT64_MAX - value->fee_minor) return UMI_STATUS_CAPACITY_EXCEEDED;
    *outTotal = value->fee_minor + value->tax_minor;
    return UMI_STATUS_OK;
}
