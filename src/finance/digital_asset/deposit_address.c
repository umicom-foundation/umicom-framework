/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/deposit_address.c
 *
 * PURPOSE:
 *   Assign a verified network address for deposits into a custody account.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/digital_asset/deposit_address.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_deposit_address_init(UmiDigitalDepositAddress *value, const char *account_id, const char *asset_id, const char *network_id, const char *address)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->account_id.value, sizeof value->account_id.value, account_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->asset_id.value, sizeof value->asset_id.value, asset_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->network_id.value, sizeof value->network_id.value, network_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->address, sizeof value->address, address);
    if (status != UMI_STATUS_OK) return status;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_deposit_address_valid(const UmiDigitalDepositAddress *value)
{
    return value != NULL && (umi_digital_asset_text_valid(value->account_id.value) && umi_digital_asset_text_valid(value->asset_id.value) && umi_digital_asset_text_valid(value->address) && value->active);
}
