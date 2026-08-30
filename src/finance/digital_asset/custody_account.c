/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/custody_account.c
 *
 * PURPOSE:
 *   Implement a client or house digital-asset custody account bound to a wallet.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/digital_asset/custody_account.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_custody_account_init(UmiDigitalCustodyAccount *value, const char *id, const UmiFinancialId *owner_party_id, const char *wallet_id, bool segregated)
{
    UmiStatus status;
    if (value == NULL || owner_party_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->wallet_id.value, sizeof value->wallet_id.value, wallet_id);
    if (status != UMI_STATUS_OK) return status;
    value->owner_party_id = *owner_party_id;
    value->segregated = segregated;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_custody_account_valid(const UmiDigitalCustodyAccount *value)
{
    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && umi_digital_asset_text_valid(value->wallet_id.value) && value->active);
}
