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
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif
