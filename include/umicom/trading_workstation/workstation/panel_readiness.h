/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_workstation/workstation/panel_readiness.h
 *
 * PURPOSE:
 *   Evaluate Panel Readiness for the professional trading workstation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_WORKSTATION_WORKSTATION_PANEL_READINESS_H
#define UMICOM_TRADING_WORKSTATION_WORKSTATION_PANEL_READINESS_H

#include "umicom/trading_workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_trading_professional_evaluate_workstation_panel_readiness(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
