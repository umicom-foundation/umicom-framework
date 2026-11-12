/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_order_throttle_boundaries.c
 *
 * PURPOSE:
 *   Verify zero-time windows, admitted arithmetic and unchanged throttle refusals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_trading_common.h"

#define CHECK(expression) do { \
    if (!(expression)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
        return 1; \
    } \
} while (0)

static int Throttle(void)
{
    UmiOrderThrottle throttle;
    umi_order_throttle_init(&throttle, 2U, 1000);
    CHECK(umi_order_throttle_accept(&throttle, 0));
    CHECK(umi_order_throttle_accept(&throttle, 0));
    UmiOrderThrottle before;
    memcpy(&before, &throttle, sizeof(before));
    CHECK(!umi_order_throttle_accept(&throttle, 0));
    CHECK(memcmp(&before, &throttle, sizeof(throttle)) == 0);
    CHECK(!umi_order_throttle_accept(&throttle, 999));
    CHECK(memcmp(&before, &throttle, sizeof(throttle)) == 0);
    CHECK(umi_order_throttle_accept(&throttle, 1000));
    CHECK(throttle.count == 1U && throttle.window_start_ms == 1000);
    memcpy(&before, &throttle, sizeof(before));
    CHECK(!umi_order_throttle_accept(&throttle, 999));
    CHECK(!umi_order_throttle_accept(&throttle, INT64_MIN));
    CHECK(memcmp(&before, &throttle, sizeof(throttle)) == 0);

    umi_order_throttle_init(&throttle, 1U, INT64_MAX);
    CHECK(umi_order_throttle_accept(&throttle, 0));
    CHECK(!umi_order_throttle_accept(&throttle, INT64_MAX - 1));
    CHECK(umi_order_throttle_accept(&throttle, INT64_MAX));
    CHECK(!umi_order_throttle_accept(&throttle, INT64_MAX));
    throttle.window_start_ms = INT64_MIN;
    memcpy(&before, &throttle, sizeof(before));
    CHECK(!umi_order_throttle_accept(&throttle, INT64_MAX));
    CHECK(memcmp(&before, &throttle, sizeof(throttle)) == 0);

    umi_order_throttle_init(&throttle, UINT32_MAX, 1);
    throttle.count = UINT32_MAX - 1U;
    CHECK(umi_order_throttle_accept(&throttle, 0));
    CHECK(throttle.count == UINT32_MAX);
    CHECK(!umi_order_throttle_accept(&throttle, 0));
    CHECK(umi_order_throttle_accept(&throttle, 1));
    CHECK(throttle.count == 1U);

    umi_order_throttle_init(&throttle, 0U, 1000);
    memcpy(&before, &throttle, sizeof(before));
    CHECK(!umi_order_throttle_accept(&throttle, 0));
    CHECK(memcmp(&before, &throttle, sizeof(throttle)) == 0);
    umi_order_throttle_init(&throttle, 2U, 0);
    CHECK(!umi_order_throttle_accept(&throttle, 0));
    umi_order_throttle_init(&throttle, 2U, -1);
    CHECK(!umi_order_throttle_accept(&throttle, 0));
    umi_order_throttle_init(&throttle, 2U, 1000);
    throttle.count = 3U;
    memcpy(&before, &throttle, sizeof(before));
    CHECK(!umi_order_throttle_accept(&throttle, 5000));
    CHECK(memcmp(&before, &throttle, sizeof(throttle)) == 0);
    CHECK(!umi_order_throttle_accept(NULL, 0));
    return 0;
}

static int OmsAttemptQuota(void)
{
    UmiOms *oms = calloc(1U, sizeof(*oms));
    CHECK(oms != NULL);
    const UmiRiskLimit limit = {5.0, 1000000.0, 10.0, 5000.0};
    umi_oms_init(oms, limit);
    umi_order_throttle_init(&oms->throttle, 1U, 1000);
    UmiOrderRequest request = test_order_request();
    UmiRiskDecision decision = {0};
    request.quantity = 0.0;
    CHECK(umi_oms_submit(oms, &request, 0.0, 0.0, 0, &decision) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!decision.allowed && oms->throttle.count == 1U && oms->orders.count == 0U);
    request.quantity = 1.0;
    CHECK(umi_oms_submit(oms, &request, 0.0, 0.0, 0, &decision) == UMI_STATUS_BUSY);
    CHECK(!decision.allowed && oms->orders.count == 0U);
    CHECK(umi_oms_submit(oms, &request, 0.0, 0.0, 1000, &decision) == UMI_STATUS_OK);
    CHECK(decision.allowed && oms->orders.count == 1U);
    umi_kill_switch_engage(&oms->kill_switch, "test");
    UmiOrderThrottle before;
    memcpy(&before, &oms->throttle, sizeof(before));
    CHECK(umi_oms_submit(oms, &request, 0.0, 0.0, 2000, &decision) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(&before, &oms->throttle, sizeof(before)) == 0);
    free(oms);
    return 0;
}

int main(void)
{
    if (Throttle() != 0) return 1;
    return OmsAttemptQuota();
}
