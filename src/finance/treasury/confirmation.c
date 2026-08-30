/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/confirmation.c
 *
 * PURPOSE:
 *   Implement represent trade confirmation terms and confirmation state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/confirmation.h"
#include <string.h>
UmiStatus umi_treasury_confirmation_init(UmiTreasuryConfirmation *value,
    const char *id,
    const char *trade_id,
    bool sent,
    bool acknowledged) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    status=umi_treasury_id_copy(value->trade_id,sizeof value->trade_id,trade_id);
    if(status!=UMI_STATUS_OK)return status;
    value->sent=sent;
    value->acknowledged=acknowledged;
    return umi_treasury_confirmation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_confirmation_valid(const UmiTreasuryConfirmation *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && umi_treasury_id_valid(value->trade_id) && (!value->acknowledged || value->sent));
}

bool umi_treasury_confirmation_complete(const UmiTreasuryConfirmation *value) {
    if (value == NULL) return (bool)0;
    return value->sent && value->acknowledged;
}
