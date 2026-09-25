/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_workstation/chart_tape/momentum.h
 *
 * PURPOSE:
 *   Evaluate Chart Momentum for the professional trading workstation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_WORKSTATION_CHART_TAPE_MOMENTUM_H
#define UMICOM_TRADING_WORKSTATION_CHART_TAPE_MOMENTUM_H

#include "umicom/trading_workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_trading_professional_evaluate_chart_tape_momentum(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
