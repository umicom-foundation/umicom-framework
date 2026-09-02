/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/automation_schedule.h
 *
 * PURPOSE:
 *   Decide when changed source code should be verified, built, tested and
 *   deployed without placing timing policy inside Studio or another client.
 *
 * ARCHITECTURE:
 *   A host records file changes and asks for the next due action. The host
 *   performs that action with Framework services and reports its result. This
 *   keeps the clock-driven policy deterministic, reusable and easy to test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_AUTOMATION_SCHEDULE_H
#define UMICOM_BUILD_AUTOMATION_SCHEDULE_H

#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_AUTOMATION_SCHEDULE_API_VERSION 1U
#define UMI_BUILD_AUTOMATION_DEFAULT_CONFIG ".umicom/automation.conf"

/* These phases make waiting, verification and compilation visibly distinct. */
typedef enum UmiBuildAutomationSchedulePhase {
    UMI_BUILD_AUTOMATION_SCHEDULE_IDLE = 0,
    UMI_BUILD_AUTOMATION_SCHEDULE_WAITING_FOR_VERIFICATION = 1,
    UMI_BUILD_AUTOMATION_SCHEDULE_VERIFYING = 2,
    UMI_BUILD_AUTOMATION_SCHEDULE_WAITING_FOR_BUILD = 3,
    UMI_BUILD_AUTOMATION_SCHEDULE_BUILDING = 4,
    UMI_BUILD_AUTOMATION_SCHEDULE_BLOCKED = 5
} UmiBuildAutomationSchedulePhase;

/* The scheduler returns work to its host instead of running tools itself. */
typedef enum UmiBuildAutomationScheduleAction {
    UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_NONE = 0,
    UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_VERIFY = 1,
    UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_BUILD = 2
} UmiBuildAutomationScheduleAction;

/*
 * All durations use milliseconds internally. The local configuration file
 * accepts minutes so a developer can write readable values such as 10 or 480.
 */
typedef struct UmiBuildAutomationSchedulePolicy {
    uint32_t structure_size;
    uint32_t api_version;
    uint64_t verification_quiet_ms;
    uint64_t build_delay_ms;
    uint64_t watchdog_ms;
    uint64_t build_interval_ms;
    int automatic_builds;
    int automatic_deploy;
} UmiBuildAutomationSchedulePolicy;

/*
 * The state is a value object. Applications may copy it into status models,
 * while only the functions below should be used to advance its lifecycle.
 */
typedef struct UmiBuildAutomationSchedule {
    uint32_t structure_size;
    uint32_t api_version;
    UmiBuildAutomationSchedulePolicy policy;
    UmiBuildAutomationSchedulePhase phase;
    uint64_t service_started_at_ms;
    uint64_t first_pending_change_at_ms;
    uint64_t last_change_at_ms;
    uint64_t verification_started_at_ms;
    uint64_t verification_completed_at_ms;
    uint64_t build_started_at_ms;
    uint64_t last_build_completed_at_ms;
    uint64_t change_revision;
    uint64_t verified_revision;
    uint64_t successful_builds;
    uint64_t failed_verifications;
    uint64_t failed_builds;
    int pending_changes;
    int manual_requested;
} UmiBuildAutomationSchedule;

/* Return the agreed safe defaults: verify at 10 minutes and build 20 later. */
UmiBuildAutomationSchedulePolicy
umi_build_automation_schedule_policy_default(void);

/* Reject impossible or internally inconsistent scheduling values. */
UmiStatus umi_build_automation_schedule_policy_validate(
    const UmiBuildAutomationSchedulePolicy *policy);

/*
 * Overlay a local key=value file onto an initialised policy. A missing file is
 * not an error and leaves the policy unchanged. Invalid input is fail-closed
 * and never partly changes the caller's policy.
 */
UmiStatus umi_build_automation_schedule_policy_load(
    const char *path,
    UmiBuildAutomationSchedulePolicy *in_out_policy,
    int *out_loaded);

/* Initialise one schedule using a monotonic clock value supplied by its host. */
UmiStatus umi_build_automation_schedule_init(
    UmiBuildAutomationSchedule *schedule,
    const UmiBuildAutomationSchedulePolicy *policy,
    uint64_t started_at_ms);

/* Record a save and invalidate any verification made for an older revision. */
UmiStatus umi_build_automation_schedule_record_change(
    UmiBuildAutomationSchedule *schedule,
    uint64_t observed_at_ms);

/* Request the pending generation now, bypassing waits but not verification. */
UmiStatus umi_build_automation_schedule_request_manual(
    UmiBuildAutomationSchedule *schedule);

/* Return the next action that is due at the supplied monotonic time. */
UmiBuildAutomationScheduleAction umi_build_automation_schedule_next_action(
    const UmiBuildAutomationSchedule *schedule,
    uint64_t now_ms);

/* Mark the beginning of the verification action returned by next_action. */
UmiStatus umi_build_automation_schedule_begin_verification(
    UmiBuildAutomationSchedule *schedule,
    uint64_t started_at_ms);

/* Record whether source verification approved the current change revision. */
UmiStatus umi_build_automation_schedule_complete_verification(
    UmiBuildAutomationSchedule *schedule,
    int passed,
    uint64_t completed_at_ms);

/* Mark the beginning of an approved incremental build generation. */
UmiStatus umi_build_automation_schedule_begin_build(
    UmiBuildAutomationSchedule *schedule,
    uint64_t started_at_ms);

/* Record the final result after compilation and executable tests finish. */
UmiStatus umi_build_automation_schedule_complete_build(
    UmiBuildAutomationSchedule *schedule,
    int passed,
    uint64_t completed_at_ms);

/* Return stable text for logs, status bars and automation dashboards. */
const char *umi_build_automation_schedule_phase_text(
    UmiBuildAutomationSchedulePhase phase);

/* Return stable text for the next action shown to a developer. */
const char *umi_build_automation_schedule_action_text(
    UmiBuildAutomationScheduleAction action);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_BUILD_AUTOMATION_SCHEDULE_H */
