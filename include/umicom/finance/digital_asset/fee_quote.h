/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/fee_quote.h
 *
 * PURPOSE:
 *   Define a time-bounded fee quote for a digital-asset network.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_FEE_QUOTE_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_FEE_QUOTE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDigitalFeeQuote {
    UmiDigitalAssetId network_id;
    UmiDigitalAmount estimated_fee;
    int64_t expires_time_ms;
} UmiDigitalFeeQuote;

/* Initialise a bounded fee quote record for reusable Framework workflows. */
UmiStatus umi_digital_asset_fee_quote_init(UmiDigitalFeeQuote *value, const char *network_id, int64_t fee_units, int32_t scale, const char *asset_symbol, int64_t expires_time_ms);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_fee_quote_valid(const UmiDigitalFeeQuote *value);

#ifdef __cplusplus
}
#endif

#endif
