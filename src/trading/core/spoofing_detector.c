/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/spoofing_detector.c
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
#include "umicom/trading/core/spoofing_detector.h"

bool umi_trading_spoofing_detector_detect(const UmiTradingSpoofingEvidence *evidence,uint32_t cancel_ratio_percent,uint32_t near_touch_percent){if(evidence==NULL||evidence->submitted_orders==0U||cancel_ratio_percent>100U||near_touch_percent>100U)return false;uint64_t cancelled=(uint64_t)evidence->cancelled_orders*100U;uint64_t near_touch=(uint64_t)evidence->near_touch_orders*100U;return cancelled>=(uint64_t)evidence->submitted_orders*cancel_ratio_percent&&near_touch>=(uint64_t)evidence->submitted_orders*near_touch_percent&&evidence->executed_orders<evidence->cancelled_orders;}
