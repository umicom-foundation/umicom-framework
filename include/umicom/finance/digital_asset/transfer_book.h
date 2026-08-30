/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/transfer_book.h
 *
 * PURPOSE:
 *   Define a bounded book of digital-asset transfer instructions.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_TRANSFER_BOOK_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_TRANSFER_BOOK_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"
#include "umicom/finance/digital_asset/transfer_instruction.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDigitalTransferBook {
    UmiDigitalTransferInstruction items[UMI_DIGITAL_ASSET_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiDigitalTransferBook;

/* Reset a bounded catalogue before loading reference data. */
void umi_digital_asset_transfer_book_init(UmiDigitalTransferBook *catalogue);

/* Add a unique item and advance the deterministic catalogue revision. */
UmiStatus umi_digital_asset_transfer_book_add(UmiDigitalTransferBook *catalogue, const UmiDigitalTransferInstruction *item);

/* Resolve an item by its stable Framework identifier. */
const UmiDigitalTransferInstruction *umi_digital_asset_transfer_book_find(const UmiDigitalTransferBook *catalogue, const char *id);

#ifdef __cplusplus
}
#endif

#endif
