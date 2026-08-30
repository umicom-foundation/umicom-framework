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
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTradingMarginProfile { uint32_t initial_margin_bps; uint32_t maintenance_margin_bps; uint32_t concentration_addon_bps; } UmiTradingMarginProfile;
/* Initialise and validate define conservative initial and maintenance margin ratios in basis points. */
UmiStatus umi_trading_margin_profile_init(UmiTradingMarginProfile *value,uint32_t initial_margin_bps, uint32_t maintenance_margin_bps, uint32_t concentration_addon_bps);
/* Validate the invariant set for this trading record. */
bool umi_trading_margin_profile_valid(const UmiTradingMarginProfile *value);
#ifdef __cplusplus
}
#endif
#endif
