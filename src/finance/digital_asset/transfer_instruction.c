/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/transfer_instruction.c
 *
 * PURPOSE:
 *   Implement a governed digital-asset transfer between custody or external addresses.
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

#include "umicom/finance/digital_asset/transfer_instruction.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_transfer_instruction_init(UmiDigitalTransferInstruction *value, const char *id, const char *source_account_id, const char *destination_address, int64_t units, int32_t scale, const char *asset_symbol)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || units <= 0 || scale < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->source_account_id.value, sizeof value->source_account_id.value, source_account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->destination_address, sizeof value->destination_address, destination_address);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->amount.asset_symbol, sizeof value->amount.asset_symbol, asset_symbol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->amount.units = units;
    value->amount.scale = scale;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_transfer_instruction_valid(const UmiDigitalTransferInstruction *value)
{
    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && umi_digital_asset_text_valid(value->source_account_id.value) && umi_digital_asset_text_valid(value->destination_address) && value->amount.units > 0);
}
