/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/custody_account.c
 *
 * PURPOSE:
 *   Implement model a securities custody account and segregation status.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/custody_account.h"
#include <string.h>
UmiStatus umi_treasury_custody_account_init(UmiTreasuryCustodyAccount *value,
    const char *id,
    const char *custodian_id,
    bool segregated) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    status=umi_treasury_id_copy(value->custodian_id,sizeof value->custodian_id,custodian_id);
    if(status!=UMI_STATUS_OK)return status;
    value->segregated=segregated;
    return umi_treasury_custody_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_custody_account_valid(const UmiTreasuryCustodyAccount *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && umi_treasury_id_valid(value->custodian_id));
}

bool umi_treasury_custody_account_is_segregated(const UmiTreasuryCustodyAccount *value) {
    if (value == NULL) return (bool)0;
    return value->segregated;
}
