/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/margin_profile.c
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
#include "umicom/trading/core/margin_profile.h"
#include <string.h>
/* Initialise and validate define conservative initial and maintenance margin ratios in basis points.. */
UmiStatus umi_trading_margin_profile_init(UmiTradingMarginProfile *value,uint32_t initial_margin_bps, uint32_t maintenance_margin_bps, uint32_t concentration_addon_bps) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    value->initial_margin_bps=initial_margin_bps;
    value->maintenance_margin_bps=maintenance_margin_bps;
    value->concentration_addon_bps=concentration_addon_bps;
    return umi_trading_margin_profile_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_margin_profile_valid(const UmiTradingMarginProfile *value) { return value!=NULL && (value->initial_margin_bps<=10000U && value->maintenance_margin_bps<=value->initial_margin_bps && value->concentration_addon_bps<=10000U); }
