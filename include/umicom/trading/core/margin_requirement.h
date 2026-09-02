/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/margin_requirement.h
 *
 * PURPOSE:
 *   Calculate integer notional margin requirements from bounded basis-point profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_MARGIN_REQUIREMENT_H
#define UMICOM_TRADING_CORE_MARGIN_REQUIREMENT_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/margin_profile.h"
/**
 * Represent the trading margin requirement data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingMarginRequirement { int64_t initial_minor; int64_t maintenance_minor; } UmiTradingMarginRequirement;
/* Calculate initial and maintenance requirements for positive notional minor units. */
UmiStatus umi_trading_margin_requirement_compute(const UmiTradingMarginProfile *profile,int64_t notional_minor,bool concentrated,UmiTradingMarginRequirement *out_requirement);
#ifdef __cplusplus
}
#endif
#endif
