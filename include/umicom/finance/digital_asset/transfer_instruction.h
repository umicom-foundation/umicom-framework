/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/transfer_instruction.h
 *
 * PURPOSE:
 *   Define a governed digital-asset transfer between custody or external addresses.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_TRANSFER_INSTRUCTION_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_TRANSFER_INSTRUCTION_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDigitalTransferInstruction {
    UmiDigitalAssetId id;
    UmiDigitalAssetId source_account_id;
    char destination_address[UMI_DIGITAL_ASSET_ADDRESS_CAPACITY];
    UmiDigitalAmount amount;
    bool approved;
    bool submitted;
} UmiDigitalTransferInstruction;

/* Initialise a bounded transfer instruction record for reusable Framework workflows. */
UmiStatus umi_digital_asset_transfer_instruction_init(UmiDigitalTransferInstruction *value, const char *id, const char *source_account_id, const char *destination_address, int64_t units, int32_t scale, const char *asset_symbol);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_transfer_instruction_valid(const UmiDigitalTransferInstruction *value);

#ifdef __cplusplus
}
#endif

#endif
