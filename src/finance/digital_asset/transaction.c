/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/transaction.c
 *
 * PURPOSE:
 *   Implement a provider-neutral on-chain transaction and confirmation evidence.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/digital_asset/transaction.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_transaction_init(UmiDigitalAssetTransaction *value, const char *id, const char *network_id, const char *from_address, const char *to_address, int64_t units, int32_t scale, const char *asset_symbol)
{
    UmiStatus status;
    if (value == NULL || units <= 0 || scale < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->network_id.value, sizeof value->network_id.value, network_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->from_address, sizeof value->from_address, from_address);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->to_address, sizeof value->to_address, to_address);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->amount.asset_symbol, sizeof value->amount.asset_symbol, asset_symbol);
    if (status != UMI_STATUS_OK) return status;
    value->amount.units = units;
    value->amount.scale = scale;
    value->state = UMI_DIGITAL_TX_CREATED;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_transaction_valid(const UmiDigitalAssetTransaction *value)
{
    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && umi_digital_asset_text_valid(value->network_id.value) && umi_digital_asset_text_valid(value->to_address) && value->amount.units > 0);
}
