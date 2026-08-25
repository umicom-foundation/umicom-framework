/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/netting_set.c
 *
 * PURPOSE:
 *   Implement define gross receivables and payables within a legally enforceable netting set.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/netting_set.h"
#include <string.h>
UmiStatus umi_treasury_netting_set_init(UmiTreasuryNettingSet *value,
    const char *id,
    int64_t receivable_minor,
    int64_t payable_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->receivable_minor=receivable_minor;
    value->payable_minor=payable_minor;
    return umi_treasury_netting_set_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_netting_set_valid(const UmiTreasuryNettingSet *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->receivable_minor >= 0 && value->payable_minor >= 0);
}

int64_t umi_treasury_netting_set_net_minor(const UmiTreasuryNettingSet *value) {
    if (value == NULL) return (int64_t)0;
    return value->receivable_minor - value->payable_minor;
}
