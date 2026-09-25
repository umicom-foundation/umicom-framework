/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_workstation/depth/bid_pressure.h
 *
 * PURPOSE:
 *   Evaluate Bid Pressure for the professional trading workstation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_WORKSTATION_DEPTH_BID_PRESSURE_H
#define UMICOM_TRADING_WORKSTATION_DEPTH_BID_PRESSURE_H

#include "umicom/trading_workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_trading_professional_evaluate_depth_bid_pressure(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
