/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/network_fee.h
 *
 * PURPOSE:
 *   Calculate deterministic estimated network fees from base and per-byte components.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_NETWORK_FEE_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_NETWORK_FEE_H

#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDigitalNetworkFee {
    UmiDigitalAssetId network_id;
    int64_t base_units;
    int64_t units_per_byte;
    int32_t scale;
    char asset_symbol[UMI_DIGITAL_ASSET_SYMBOL_CAPACITY];
} UmiDigitalNetworkFee;

/* Initialise a non-negative network fee schedule. */
UmiStatus umi_digital_asset_network_fee_init(UmiDigitalNetworkFee *value, const char *network_id, int64_t base_units, int64_t units_per_byte, int32_t scale, const char *asset_symbol);

/* Estimate fee units for a serialized transaction size. */
UmiStatus umi_digital_asset_network_fee_calculate(const UmiDigitalNetworkFee *value, uint32_t bytes, int64_t *out_units);

#ifdef __cplusplus
}
#endif

#endif
