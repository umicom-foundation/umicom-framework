/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/margin_requirement.c
 *
 * PURPOSE:
 *   Calculate integer notional margin requirements from bounded basis-point profiles.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/margin_requirement.h"

/* Scale a positive amount by basis points without multiplying two large values first. */
static int64_t scale_bps(int64_t amount,uint32_t bps){int64_t q=amount/10000;int64_t r=amount%10000;return q*(int64_t)bps+(r*(int64_t)bps)/10000;}
UmiStatus umi_trading_margin_requirement_compute(const UmiTradingMarginProfile *profile,int64_t notional_minor,bool concentrated,UmiTradingMarginRequirement *out_requirement){if(!umi_trading_margin_profile_valid(profile)||notional_minor<0||out_requirement==NULL)return UMI_STATUS_INVALID_ARGUMENT;uint32_t addon=concentrated?profile->concentration_addon_bps:0U;if(profile->initial_margin_bps>10000U-addon)return UMI_STATUS_CAPACITY_EXCEEDED;out_requirement->initial_minor=scale_bps(notional_minor,profile->initial_margin_bps+addon);out_requirement->maintenance_minor=scale_bps(notional_minor,profile->maintenance_margin_bps);return UMI_STATUS_OK;}
