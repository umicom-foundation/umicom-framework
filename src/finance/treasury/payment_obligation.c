/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/payment_obligation.c
 *
 * PURPOSE:
 *   Implement represent a dated treasury payment obligation and outstanding amount.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/payment_obligation.h"
#include <string.h>
UmiStatus umi_treasury_payment_obligation_init(UmiTreasuryPaymentObligation *value,
    const char *id,
    int64_t due_epoch_millis,
    int64_t amount_minor,
    int64_t paid_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->due_epoch_millis=due_epoch_millis;
    value->amount_minor=amount_minor;
    value->paid_minor=paid_minor;
    return umi_treasury_payment_obligation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_payment_obligation_valid(const UmiTreasuryPaymentObligation *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->due_epoch_millis >= 0 && value->amount_minor >= 0 && value->paid_minor >= 0 && value->paid_minor <= value->amount_minor);
}

int64_t umi_treasury_payment_obligation_outstanding_minor(const UmiTreasuryPaymentObligation *value) {
    if (value == NULL) return (int64_t)0;
    return value->amount_minor - value->paid_minor;
}
