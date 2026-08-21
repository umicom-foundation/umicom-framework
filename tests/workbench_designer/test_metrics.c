/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_metrics.c
 *
 * PURPOSE:
 *   Verify designer operational counters and derived command success and latency
 *   metrics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchDesignerMetrics metrics;
    double success_rate;
    double average_duration;

    umi_workbench_designer_metrics_init(&metrics);
    umi_workbench_designer_metrics_record_command(
        &metrics, UMI_STATUS_OK, 100U, false, false);
    umi_workbench_designer_metrics_record_command(
        &metrics, UMI_STATUS_INVALID_STATE, 300U, false, false);
    umi_workbench_designer_metrics_record_command(
        &metrics, UMI_STATUS_OK, 200U, true, false);
    umi_workbench_designer_metrics_record_save(&metrics, UMI_STATUS_OK);
    umi_workbench_designer_metrics_record_save(
        &metrics, UMI_STATUS_IO_ERROR);
    TEST_REQUIRE(metrics.commands_executed == 3U);
    TEST_REQUIRE(metrics.commands_failed == 1U);
    TEST_REQUIRE(metrics.undo_count == 1U);
    TEST_REQUIRE(metrics.maximum_command_duration_us == 300U);
    TEST_REQUIRE(metrics.saves_started == 2U);
    TEST_REQUIRE(metrics.saves_completed == 1U);
    TEST_REQUIRE(metrics.saves_failed == 1U);
    success_rate = umi_workbench_designer_metrics_command_success_rate(&metrics);
    average_duration =
        umi_workbench_designer_metrics_average_command_duration_us(&metrics);
    TEST_REQUIRE(success_rate > 0.66 && success_rate < 0.67);
    TEST_REQUIRE(average_duration == 200.0);
    return 0;
}
