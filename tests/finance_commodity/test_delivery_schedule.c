/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_delivery_schedule.c
 *
 * PURPOSE:
 *   Implement the test delivery schedule behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/delivery_schedule.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCommodityDeliverySchedule schedule;
    UmiCommodityDeliveryObligation first;
    UmiCommodityDeliveryObligation second;
    CHECK(umi_commodity_delivery_schedule_init(&schedule, "CTR-1") == UMI_STATUS_OK);
    CHECK(umi_commodity_delivery_obligation_init(&first, "DEL-1", "CTR-1", 100, 0, "MT", 1000) == UMI_STATUS_OK);
    CHECK(umi_commodity_delivery_obligation_init(&second, "DEL-2", "CTR-1", 100, 0, "MT", 2000) == UMI_STATUS_OK);
    CHECK(umi_commodity_delivery_schedule_add(&schedule, &first) == UMI_STATUS_OK);
    CHECK(umi_commodity_delivery_schedule_add(&schedule, &second) == UMI_STATUS_OK);
    CHECK(schedule.count == 2U);
    return 0;
}
