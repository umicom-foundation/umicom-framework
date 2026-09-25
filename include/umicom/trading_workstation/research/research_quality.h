/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_workstation/research/research_quality.h
 *
 * PURPOSE:
 *   Evaluate Research Evidence Quality for the professional trading workstation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_WORKSTATION_RESEARCH_RESEARCH_QUALITY_H
#define UMICOM_TRADING_WORKSTATION_RESEARCH_RESEARCH_QUALITY_H

#include "umicom/trading_workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_trading_professional_evaluate_research_research_quality(
    const UmiTradingProfessionalInput *input,
    UmiTradingProfessionalSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
