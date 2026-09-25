/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/study_series.h
 *
 * PURPOSE:
 *   Compute deterministic non-rendering strategy studies over bounded price and
 *   volume series for backtests, replay and Studio research tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_STUDY_SERIES_H
#define UMICOM_STRATEGY_RESEARCH_STUDY_SERIES_H

#include <stddef.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_study_sma(
    const double *values,
    size_t count,
    size_t period,
    double *outValue);

UmiStatus umi_strategy_study_ema(
    const double *values,
    size_t count,
    size_t period,
    double *outValue);

UmiStatus umi_strategy_study_rsi(
    const double *values,
    size_t count,
    size_t period,
    double *outValue);

UmiStatus umi_strategy_study_atr(
    const double *high,
    const double *low,
    const double *close,
    size_t count,
    size_t period,
    double *outValue);

UmiStatus umi_strategy_study_vwap(
    const double *price,
    const double *volume,
    size_t count,
    double *outValue);

#ifdef __cplusplus
}
#endif
#endif
