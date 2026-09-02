/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/spoofing_detector.h
 *
 * PURPOSE:
 *   Detect high cancellation ratios near the touch as a conservative spoofing signal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_SPOOFING_DETECTOR_H
#define UMICOM_TRADING_CORE_SPOOFING_DETECTOR_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the trading spoofing evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingSpoofingEvidence { uint32_t submitted_orders; uint32_t cancelled_orders; uint32_t executed_orders; uint32_t near_touch_orders; } UmiTradingSpoofingEvidence;
/* Flag evidence when cancellation and near-touch concentration exceed configured percentages. */
bool umi_trading_spoofing_detector_detect(const UmiTradingSpoofingEvidence *evidence,uint32_t cancel_ratio_percent,uint32_t near_touch_percent);
#ifdef __cplusplus
}
#endif
#endif
