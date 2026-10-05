/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/digital_market.h
 *
 * PURPOSE:
 *   Define a digital-asset market pair that can be routed through canonical trading services.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_DIGITAL_MARKET_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_DIGITAL_MARKET_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the digital market data shared with callers of this public contract.
 */
typedef struct UmiDigitalMarket {
    UmiDigitalAssetId id;
    UmiDigitalAssetId base_asset_id;
    UmiDigitalAssetId quote_asset_id;
    char venue[UMI_DIGITAL_ASSET_NAME_CAPACITY];
    int64_t minimum_quantity_units;
    bool active;
} UmiDigitalMarket;

/* Initialise a bounded digital market record for reusable Framework workflows. */
UmiStatus umi_digital_asset_digital_market_init(UmiDigitalMarket *value, const char *id, const char *base_asset_id, const char *quote_asset_id, const char *venue, int64_t minimum_quantity_units);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_digital_market_valid(const UmiDigitalMarket *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_digital_asset_digital_market_archive_encode(const UmiDigitalMarket *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_digital_asset_digital_market_archive_decode(const void *bytes, size_t byte_count,
    UmiDigitalMarket *value);

#ifdef __cplusplus
}
#endif

#endif
