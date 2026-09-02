/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_automation_schedule.c
 *
 * PURPOSE:
 *   Verify default, manual, watchdog, interval and configuration-file timing
 *   without waiting for real time or starting a compiler.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/build/automation_schedule.h"

#define MINUTE_MS UINT64_C(60000)

/* Verify the agreed ten-minute scan and twenty-minute post-scan build delay. */
static void test_default_automatic_schedule(void)
{
    UmiBuildAutomationSchedule schedule;
    UmiBuildAutomationSchedulePolicy policy =
        umi_build_automation_schedule_policy_default();

    assert(umi_build_automation_schedule_init(&schedule, &policy, 0U) ==
           UMI_STATUS_OK);
    assert(umi_build_automation_schedule_record_change(
               &schedule, MINUTE_MS) == UMI_STATUS_OK);
    assert(umi_build_automation_schedule_next_action(
               &schedule, UINT64_C(10) * MINUTE_MS) ==
           UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_NONE);
    assert(umi_build_automation_schedule_next_action(
               &schedule, UINT64_C(11) * MINUTE_MS) ==
           UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_VERIFY);
    assert(umi_build_automation_schedule_begin_verification(
               &schedule, UINT64_C(11) * MINUTE_MS) == UMI_STATUS_OK);
    assert(umi_build_automation_schedule_complete_verification(
               &schedule, 1, UINT64_C(12) * MINUTE_MS) == UMI_STATUS_OK);
    assert(umi_build_automation_schedule_next_action(
               &schedule, UINT64_C(31) * MINUTE_MS) ==
           UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_NONE);
    assert(umi_build_automation_schedule_next_action(
               &schedule, UINT64_C(32) * MINUTE_MS) ==
           UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_BUILD);
}

/* Verify that a developer can build now without bypassing verification. */
static void test_manual_request_keeps_gate(void)
{
    UmiBuildAutomationSchedule schedule;

    assert(umi_build_automation_schedule_init(&schedule, NULL, 100U) ==
           UMI_STATUS_OK);
    assert(umi_build_automation_schedule_record_change(&schedule, 200U) ==
           UMI_STATUS_OK);
    assert(umi_build_automation_schedule_request_manual(&schedule) ==
           UMI_STATUS_OK);
    assert(umi_build_automation_schedule_next_action(&schedule, 200U) ==
           UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_VERIFY);
    assert(umi_build_automation_schedule_begin_verification(&schedule, 200U) ==
           UMI_STATUS_OK);
    assert(umi_build_automation_schedule_complete_verification(
               &schedule, 1, 300U) == UMI_STATUS_OK);
    assert(umi_build_automation_schedule_next_action(&schedule, 300U) ==
           UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_BUILD);
    assert(umi_build_automation_schedule_begin_build(&schedule, 300U) ==
           UMI_STATUS_OK);
    assert(umi_build_automation_schedule_complete_build(
               &schedule, 1, 400U) == UMI_STATUS_OK);
    assert(schedule.phase == UMI_BUILD_AUTOMATION_SCHEDULE_IDLE);
    assert(schedule.successful_builds == 1U);
}

/* Verify that changed code revokes an approval made for an older revision. */
static void test_new_change_restarts_verification(void)
{
    UmiBuildAutomationSchedule schedule;

    assert(umi_build_automation_schedule_init(&schedule, NULL, 0U) ==
           UMI_STATUS_OK);
    assert(umi_build_automation_schedule_record_change(&schedule, 100U) ==
           UMI_STATUS_OK);
    assert(umi_build_automation_schedule_request_manual(&schedule) ==
           UMI_STATUS_OK);
    assert(umi_build_automation_schedule_begin_verification(&schedule, 100U) ==
           UMI_STATUS_OK);
    assert(umi_build_automation_schedule_complete_verification(
               &schedule, 1, 200U) == UMI_STATUS_OK);
    assert(umi_build_automation_schedule_record_change(&schedule, 300U) ==
           UMI_STATUS_OK);
    assert(schedule.verified_revision == 0U);
    assert(umi_build_automation_schedule_next_action(&schedule, 300U) ==
           UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_VERIFY);
}

/* Verify an eight-hour interval holds approved work until its next boundary. */
static void test_interval_schedule(void)
{
    UmiBuildAutomationSchedule schedule;
    UmiBuildAutomationSchedulePolicy policy =
        umi_build_automation_schedule_policy_default();

    policy.build_interval_ms = UINT64_C(480) * MINUTE_MS;
    assert(umi_build_automation_schedule_init(&schedule, &policy, 0U) ==
           UMI_STATUS_OK);
    assert(umi_build_automation_schedule_record_change(
               &schedule, MINUTE_MS) == UMI_STATUS_OK);
    assert(umi_build_automation_schedule_begin_verification(
               &schedule, UINT64_C(11) * MINUTE_MS) == UMI_STATUS_OK);
    assert(umi_build_automation_schedule_complete_verification(
               &schedule, 1, UINT64_C(12) * MINUTE_MS) == UMI_STATUS_OK);
    assert(umi_build_automation_schedule_next_action(
               &schedule, UINT64_C(479) * MINUTE_MS) ==
           UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_NONE);
    assert(umi_build_automation_schedule_next_action(
               &schedule, UINT64_C(480) * MINUTE_MS) ==
           UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_BUILD);
}

/* Verify failed scans block builds until a repair produces a new revision. */
static void test_failed_verification_blocks(void)
{
    UmiBuildAutomationSchedule schedule;

    assert(umi_build_automation_schedule_init(&schedule, NULL, 0U) ==
           UMI_STATUS_OK);
    assert(umi_build_automation_schedule_record_change(&schedule, 100U) ==
           UMI_STATUS_OK);
    assert(umi_build_automation_schedule_request_manual(&schedule) ==
           UMI_STATUS_OK);
    assert(umi_build_automation_schedule_begin_verification(&schedule, 100U) ==
           UMI_STATUS_OK);
    assert(umi_build_automation_schedule_complete_verification(
               &schedule, 0, 200U) == UMI_STATUS_PERMISSION_DENIED);
    assert(schedule.phase == UMI_BUILD_AUTOMATION_SCHEDULE_BLOCKED);
    assert(umi_build_automation_schedule_next_action(&schedule, 300U) ==
           UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_NONE);
    assert(umi_build_automation_schedule_record_change(&schedule, 400U) ==
           UMI_STATUS_OK);
    assert(schedule.phase ==
           UMI_BUILD_AUTOMATION_SCHEDULE_WAITING_FOR_VERIFICATION);
}

/* Verify valid local settings are read and malformed settings are fail-closed. */
static void test_configuration_file(void)
{
    static const char valid_path[] = "umicom-automation-schedule-test.conf";
    static const char invalid_path[] =
        "umicom-automation-schedule-invalid-test.conf";
    UmiBuildAutomationSchedulePolicy policy =
        umi_build_automation_schedule_policy_default();
    FILE *stream;
    int loaded = 0;

    stream = fopen(valid_path, "wb");
    assert(stream != NULL);
    assert(fputs("# Local automated build timing.\n"
                 "verification_quiet_minutes=15\n"
                 "build_delay_minutes=30\n"
                 "watchdog_minutes=90\n"
                 "build_interval_minutes=480\n"
                 "automatic_builds=true\n"
                 "automatic_deploy=true\n",
                 stream) >= 0);
    assert(fclose(stream) == 0);
    assert(umi_build_automation_schedule_policy_load(
               valid_path, &policy, &loaded) == UMI_STATUS_OK);
    assert(loaded != 0);
    assert(policy.verification_quiet_ms == UINT64_C(15) * MINUTE_MS);
    assert(policy.build_interval_ms == UINT64_C(480) * MINUTE_MS);
    assert(policy.automatic_deploy != 0);
    assert(remove(valid_path) == 0);

    stream = fopen(invalid_path, "wb");
    assert(stream != NULL);
    assert(fputs("build_delai_minutes=30\n", stream) >= 0);
    assert(fclose(stream) == 0);
    assert(umi_build_automation_schedule_policy_load(
               invalid_path, &policy, &loaded) == UMI_STATUS_PARSE_ERROR);
    assert(loaded == 0);
    assert(policy.build_delay_ms == UINT64_C(30) * MINUTE_MS);
    assert(remove(invalid_path) == 0);
}

/* Execute deterministic scheduler scenarios without starting external tools. */
int main(void)
{
    test_default_automatic_schedule();
    test_manual_request_keeps_gate();
    test_new_change_restarts_verification();
    test_interval_schedule();
    test_failed_verification_blocks();
    test_configuration_file();
    assert(strcmp(umi_build_automation_schedule_action_text(
                      UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_VERIFY),
                  "verify") == 0);
    return 0;
}
