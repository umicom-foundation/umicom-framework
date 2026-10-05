/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/margin_profile.h
 *
 * PURPOSE:
 *   Define conservative initial and maintenance margin ratios in basis points.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_MARGIN_PROFILE_H
#define UMICOM_TRADING_CORE_MARGIN_PROFILE_H
#include "umicom/trading/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trading margin profile data shared with callers of this public contract.
 */
typedef struct UmiTradingMarginProfile { uint32_t initial_margin_bps; uint32_t maintenance_margin_bps; uint32_t concentration_addon_bps; } UmiTradingMarginProfile;
/* Initialise and validate define conservative initial and maintenance margin ratios in basis points. */
UmiStatus umi_trading_margin_profile_init(UmiTradingMarginProfile *value,uint32_t initial_margin_bps, uint32_t maintenance_margin_bps, uint32_t concentration_addon_bps);
/* Validate the invariant set for this trading record. */
bool umi_trading_margin_profile_valid(const UmiTradingMarginProfile *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_trading_margin_profile_archive_encode(const UmiTradingMarginProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_trading_margin_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiTradingMarginProfile *value);

#ifdef __cplusplus
}
#endif
#endif
