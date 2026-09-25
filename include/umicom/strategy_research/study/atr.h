/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/study/atr.h
 *
 * PURPOSE:
 *   Evaluate Average True Range as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_STUDY_ATR_H
#define UMICOM_STRATEGY_RESEARCH_STUDY_ATR_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_study_atr(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
