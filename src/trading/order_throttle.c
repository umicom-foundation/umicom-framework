/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/order_throttle.c
 *
 * PURPOSE:
 *   Apply a simple bounded order-rate limit per time window.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of order throttle. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/order_throttle.h"

/*
 * Initialise order throttle from caller-provided values so later operations receive a
 * known state.
 */
void umi_order_throttle_init(UmiOrderThrottle *throttle,
                             uint32_t max_count,
                             int64_t window_ms)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (throttle == NULL) {
        return;
    }

    throttle->count = 0U;
    throttle->max_count = max_count;
    throttle->window_start_ms = 0;
    throttle->window_ms = window_ms;
}

/*
 * Provide the order throttle accept operation used by this module and its client
 * applications.
 */
/* Use the accepted count to distinguish an unused time window, admit time arithmetic and publish only accepted attempts. This replaces the zero-timestamp sentinel and retains its implementation for review. The previous implementation is retained for review. */
#if 0
int umi_order_throttle_accept(UmiOrderThrottle *throttle, int64_t now_ms)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (throttle == NULL ||
        throttle->max_count == 0U ||
        throttle->window_ms <= 0) {
        return 0;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (throttle->window_start_ms == 0 ||
        now_ms - throttle->window_start_ms >= throttle->window_ms) {
        throttle->window_start_ms = now_ms;
        throttle->count = 0U;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (throttle->count >= throttle->max_count) {
        return 0;
    }

    throttle->count++;
    return 1;
}
#endif
int umi_order_throttle_accept(UmiOrderThrottle *throttle, int64_t now_ms)
{
    if (throttle == NULL || throttle->max_count == 0U ||
        throttle->window_ms <= 0 || throttle->window_start_ms < 0 ||
        now_ms < 0 || throttle->count > throttle->max_count)
        return 0;
    if (throttle->count != 0U && now_ms < throttle->window_start_ms)
        return 0;
    UmiOrderThrottle next = *throttle;
    /* Count zero identifies an unused throttle; timestamp zero is valid.
     * With nonnegative ordered operands, subtraction cannot overflow int64_t. */
    if (next.count == 0U || now_ms - next.window_start_ms >= next.window_ms) {
        next.window_start_ms = now_ms;
        next.count = 0U;
    }
    if (next.count >= next.max_count) return 0;
    next.count += 1U;
    *throttle = next;
    return 1;
}

