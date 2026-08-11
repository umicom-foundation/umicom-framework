/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/order_throttle.c
 *
 * PURPOSE:
 *   Apply a simple bounded order-rate limit per time window.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of order throttle. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/order_throttle.h"

void umi_order_throttle_init(UmiOrderThrottle *throttle,
                             uint32_t max_count,
                             int64_t window_ms)
{
    if (throttle == NULL) {
        return;
    }

    throttle->count = 0U;
    throttle->max_count = max_count;
    throttle->window_start_ms = 0;
    throttle->window_ms = window_ms;
}

int umi_order_throttle_accept(UmiOrderThrottle *throttle, int64_t now_ms)
{
    if (throttle == NULL ||
        throttle->max_count == 0U ||
        throttle->window_ms <= 0) {
        return 0;
    }

    if (throttle->window_start_ms == 0 ||
        now_ms - throttle->window_start_ms >= throttle->window_ms) {
        throttle->window_start_ms = now_ms;
        throttle->count = 0U;
    }

    if (throttle->count >= throttle->max_count) {
        return 0;
    }

    throttle->count++;
    return 1;
}
