/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/automation_schedule.c
 *
 * PURPOSE:
 *   Implement configurable quiet-time, verification, scheduled-build and
 *   manual-build decisions for the Umicom Automated Build System.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/automation_schedule.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_BUILD_MINUTE_MS UINT64_C(60000)

/* Add a duration without allowing an unsigned clock value to wrap. */
static uint64_t schedule_due_at(uint64_t started_at_ms, uint64_t delay_ms)
{
    if (UINT64_MAX - started_at_ms < delay_ms) {
        return UINT64_MAX;
    }
    return started_at_ms + delay_ms;
}

/* Return true only when a monotonic deadline has been reached safely. */
static int schedule_due(uint64_t now_ms, uint64_t deadline_ms)
{
    return deadline_ms != 0U && now_ms >= deadline_ms;
}

/* Remove surrounding whitespace without allocating another text buffer. */
static char *schedule_trim(char *text)
{
    char *end;

    if (text == NULL) {
        return NULL;
    }
    while (*text != '\0' && isspace((unsigned char)*text)) {
        ++text;
    }
    end = text + strlen(text);
    while (end > text && isspace((unsigned char)end[-1])) {
        --end;
    }
    *end = '\0';
    return text;
}

/* Parse a whole number of minutes and convert it with overflow checks. */
static UmiStatus schedule_parse_minutes(const char *text,
                                        uint64_t *out_milliseconds)
{
    char *end = NULL;
    unsigned long long minutes;

    if (text == NULL || out_milliseconds == NULL || text[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    errno = 0;
    minutes = strtoull(text, &end, 10);
    if (errno == ERANGE || end == text || end == NULL || *end != '\0' ||
        minutes > UINT64_MAX / UMI_BUILD_MINUTE_MS) {
        return UMI_STATUS_PARSE_ERROR;
    }
    *out_milliseconds = (uint64_t)minutes * UMI_BUILD_MINUTE_MS;
    return UMI_STATUS_OK;
}

/* Parse explicit boolean words so misspelled policy values cannot be ignored. */
static UmiStatus schedule_parse_boolean(const char *text, int *out_value)
{
    if (text == NULL || out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(text, "true") == 0 || strcmp(text, "yes") == 0 ||
        strcmp(text, "1") == 0) {
        *out_value = 1;
        return UMI_STATUS_OK;
    }
    if (strcmp(text, "false") == 0 || strcmp(text, "no") == 0 ||
        strcmp(text, "0") == 0) {
        *out_value = 0;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_PARSE_ERROR;
}

/* Apply one recognised configuration field to a temporary policy copy. */
static UmiStatus schedule_apply_setting(
    UmiBuildAutomationSchedulePolicy *policy,
    const char *key,
    const char *value)
{
    if (strcmp(key, "verification_quiet_minutes") == 0) {
        return schedule_parse_minutes(value, &policy->verification_quiet_ms);
    }
    if (strcmp(key, "build_delay_minutes") == 0) {
        return schedule_parse_minutes(value, &policy->build_delay_ms);
    }
    if (strcmp(key, "watchdog_minutes") == 0) {
        return schedule_parse_minutes(value, &policy->watchdog_ms);
    }
    if (strcmp(key, "build_interval_minutes") == 0) {
        return schedule_parse_minutes(value, &policy->build_interval_ms);
    }
    if (strcmp(key, "automatic_builds") == 0) {
        return schedule_parse_boolean(value, &policy->automatic_builds);
    }
    if (strcmp(key, "automatic_deploy") == 0) {
        return schedule_parse_boolean(value, &policy->automatic_deploy);
    }
    /* Unknown keys are rejected because silently accepting a spelling error
     * could make a developer believe a safety delay or gate is active. */
    return UMI_STATUS_PARSE_ERROR;
}

/* Calculate the automatic verification deadline for the pending revision. */
static uint64_t schedule_verification_due_at(
    const UmiBuildAutomationSchedule *schedule)
{
    uint64_t deadline = schedule_due_at(
        schedule->last_change_at_ms,
        schedule->policy.verification_quiet_ms);

    /* In normal quiet-time mode, the watchdog prevents a pending generation
     * from being forgotten. Interval mode deliberately honours its longer
     * developer-selected schedule instead of forcing an early build. */
    if (schedule->policy.build_interval_ms == 0U &&
        schedule->policy.watchdog_ms != 0U) {
        const uint64_t watchdog = schedule_due_at(
            schedule->first_pending_change_at_ms,
            schedule->policy.watchdog_ms);

        if (watchdog < deadline) {
            deadline = watchdog;
        }
    }
    return deadline;
}

/* Calculate when an approved revision may enter its incremental build. */
static uint64_t schedule_build_due_at(
    const UmiBuildAutomationSchedule *schedule)
{
    uint64_t deadline;

    if (schedule->policy.build_interval_ms != 0U) {
        const uint64_t anchor = schedule->last_build_completed_at_ms != 0U
            ? schedule->last_build_completed_at_ms
            : schedule->service_started_at_ms;

        deadline = schedule_due_at(
            anchor, schedule->policy.build_interval_ms);
        /* A schedule boundary that passed while code was still being checked
         * becomes ready immediately after verification, never before it. */
        if (deadline < schedule->verification_completed_at_ms) {
            deadline = schedule->verification_completed_at_ms;
        }
        return deadline;
    }

    deadline = schedule_due_at(schedule->verification_completed_at_ms,
                               schedule->policy.build_delay_ms);
    if (schedule->policy.watchdog_ms != 0U) {
        const uint64_t watchdog = schedule_due_at(
            schedule->first_pending_change_at_ms,
            schedule->policy.watchdog_ms);

        if (watchdog < deadline) {
            deadline = watchdog;
        }
    }
    /* A watchdog may expire during a long scan, but it must never permit a
     * build before that scan has produced successful verification evidence. */
    if (deadline < schedule->verification_completed_at_ms) {
        deadline = schedule->verification_completed_at_ms;
    }
    return deadline;
}

/* Return the agreed timing and deployment defaults for a local workspace. */
UmiBuildAutomationSchedulePolicy
umi_build_automation_schedule_policy_default(void)
{
    UmiBuildAutomationSchedulePolicy policy;

    (void)memset(&policy, 0, sizeof(policy));
    policy.structure_size = (uint32_t)sizeof(policy);
    policy.api_version = UMI_BUILD_AUTOMATION_SCHEDULE_API_VERSION;
    policy.verification_quiet_ms = UINT64_C(10) * UMI_BUILD_MINUTE_MS;
    policy.build_delay_ms = UINT64_C(20) * UMI_BUILD_MINUTE_MS;
    policy.watchdog_ms = UINT64_C(60) * UMI_BUILD_MINUTE_MS;
    policy.automatic_builds = 1;
    policy.automatic_deploy = 0;
    return policy;
}

/* Validate structure identity and timing rules before they control a build. */
UmiStatus umi_build_automation_schedule_policy_validate(
    const UmiBuildAutomationSchedulePolicy *policy)
{
    if (policy == NULL || policy->structure_size < sizeof(*policy) ||
        policy->api_version != UMI_BUILD_AUTOMATION_SCHEDULE_API_VERSION ||
        policy->verification_quiet_ms == 0U ||
        (policy->automatic_builds != 0 && policy->automatic_builds != 1) ||
        (policy->automatic_deploy != 0 && policy->automatic_deploy != 1)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Load a bounded local configuration without partially applying bad input. */
UmiStatus umi_build_automation_schedule_policy_load(
    const char *path,
    UmiBuildAutomationSchedulePolicy *in_out_policy,
    int *out_loaded)
{
    UmiBuildAutomationSchedulePolicy candidate;
    char line[512];
    FILE *stream;
    UmiStatus status = UMI_STATUS_OK;

    if (path == NULL || path[0] == '\0' || in_out_policy == NULL ||
        out_loaded == NULL ||
        umi_build_automation_schedule_policy_validate(in_out_policy) !=
            UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_loaded = 0;
    errno = 0;
    stream = fopen(path, "rb");
    if (stream == NULL) {
        return errno == ENOENT ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
    }
    candidate = *in_out_policy;
    while (fgets(line, sizeof(line), stream) != NULL) {
        char *key = schedule_trim(line);
        char *equals;
        char *value;

        if (key[0] == '\0' || key[0] == '#' || key[0] == ';') {
            continue;
        }
        equals = strchr(key, '=');
        if (equals == NULL) {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        *equals = '\0';
        value = schedule_trim(equals + 1);
        key = schedule_trim(key);
        if (key[0] == '\0' || value[0] == '\0') {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        status = schedule_apply_setting(&candidate, key, value);
        if (status != UMI_STATUS_OK) {
            break;
        }
    }
    if (status == UMI_STATUS_OK && ferror(stream)) {
        status = UMI_STATUS_IO_ERROR;
    }
    (void)fclose(stream);
    if (status == UMI_STATUS_OK) {
        status = umi_build_automation_schedule_policy_validate(&candidate);
    }
    if (status == UMI_STATUS_OK) {
        *in_out_policy = candidate;
        *out_loaded = 1;
    }
    return status;
}

/* Initialise a reusable schedule at a caller-supplied monotonic start time. */
UmiStatus umi_build_automation_schedule_init(
    UmiBuildAutomationSchedule *schedule,
    const UmiBuildAutomationSchedulePolicy *policy,
    uint64_t started_at_ms)
{
    UmiBuildAutomationSchedulePolicy effective = policy != NULL
        ? *policy
        : umi_build_automation_schedule_policy_default();
    UmiStatus status =
        umi_build_automation_schedule_policy_validate(&effective);

    if (schedule == NULL || status != UMI_STATUS_OK) {
        return schedule == NULL ? UMI_STATUS_INVALID_ARGUMENT : status;
    }
    (void)memset(schedule, 0, sizeof(*schedule));
    schedule->structure_size = (uint32_t)sizeof(*schedule);
    schedule->api_version = UMI_BUILD_AUTOMATION_SCHEDULE_API_VERSION;
    schedule->policy = effective;
    schedule->phase = UMI_BUILD_AUTOMATION_SCHEDULE_IDLE;
    schedule->service_started_at_ms = started_at_ms;
    return UMI_STATUS_OK;
}

/* Restart the quiet timer and revoke approval for every older source image. */
UmiStatus umi_build_automation_schedule_record_change(
    UmiBuildAutomationSchedule *schedule,
    uint64_t observed_at_ms)
{
    if (schedule == NULL || schedule->structure_size < sizeof(*schedule) ||
        schedule->api_version != UMI_BUILD_AUTOMATION_SCHEDULE_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (schedule->phase == UMI_BUILD_AUTOMATION_SCHEDULE_VERIFYING ||
        schedule->phase == UMI_BUILD_AUTOMATION_SCHEDULE_BUILDING ||
        schedule->change_revision == UINT64_MAX) {
        return schedule->change_revision == UINT64_MAX
            ? UMI_STATUS_CAPACITY_EXCEEDED
            : UMI_STATUS_BUSY;
    }
    if (!schedule->pending_changes) {
        schedule->first_pending_change_at_ms = observed_at_ms;
    }
    schedule->pending_changes = 1;
    schedule->last_change_at_ms = observed_at_ms;
    schedule->change_revision += 1U;
    schedule->verified_revision = 0U;
    schedule->verification_completed_at_ms = 0U;
    schedule->phase =
        UMI_BUILD_AUTOMATION_SCHEDULE_WAITING_FOR_VERIFICATION;
    return UMI_STATUS_OK;
}

/* Preserve all required gates while making the next action immediately due. */
UmiStatus umi_build_automation_schedule_request_manual(
    UmiBuildAutomationSchedule *schedule)
{
    if (schedule == NULL || !schedule->pending_changes) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (schedule->phase == UMI_BUILD_AUTOMATION_SCHEDULE_VERIFYING ||
        schedule->phase == UMI_BUILD_AUTOMATION_SCHEDULE_BUILDING) {
        return UMI_STATUS_BUSY;
    }
    schedule->manual_requested = 1;
    return UMI_STATUS_OK;
}

/* Choose verification or build only when the matching lifecycle gate is due. */
UmiBuildAutomationScheduleAction umi_build_automation_schedule_next_action(
    const UmiBuildAutomationSchedule *schedule,
    uint64_t now_ms)
{
    if (schedule == NULL || !schedule->pending_changes) {
        return UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_NONE;
    }
    if (schedule->phase ==
        UMI_BUILD_AUTOMATION_SCHEDULE_WAITING_FOR_VERIFICATION) {
        if (schedule->manual_requested ||
            (schedule->policy.automatic_builds &&
             schedule_due(now_ms, schedule_verification_due_at(schedule)))) {
            return UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_VERIFY;
        }
    }
    /* A failed verification or build must not form a rapid retry loop. A new
     * source change leaves BLOCKED through record_change, while an explicit
     * manual request allows the developer to retry unchanged infrastructure. */
    if (schedule->phase == UMI_BUILD_AUTOMATION_SCHEDULE_BLOCKED &&
        schedule->manual_requested) {
        return UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_VERIFY;
    }
    if (schedule->phase ==
            UMI_BUILD_AUTOMATION_SCHEDULE_WAITING_FOR_BUILD &&
        schedule->verified_revision == schedule->change_revision) {
        if (schedule->manual_requested ||
            (schedule->policy.automatic_builds &&
             schedule_due(now_ms, schedule_build_due_at(schedule)))) {
            return UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_BUILD;
        }
    }
    return UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_NONE;
}

/* Enter verification only after the scheduler has approved that transition. */
UmiStatus umi_build_automation_schedule_begin_verification(
    UmiBuildAutomationSchedule *schedule,
    uint64_t started_at_ms)
{
    if (schedule == NULL ||
        umi_build_automation_schedule_next_action(schedule, started_at_ms) !=
            UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_VERIFY) {
        return UMI_STATUS_INVALID_STATE;
    }
    schedule->phase = UMI_BUILD_AUTOMATION_SCHEDULE_VERIFYING;
    schedule->verification_started_at_ms = started_at_ms;
    return UMI_STATUS_OK;
}

/* Approve the exact scanned revision or block it until repair/manual retry. */
UmiStatus umi_build_automation_schedule_complete_verification(
    UmiBuildAutomationSchedule *schedule,
    int passed,
    uint64_t completed_at_ms)
{
    if (schedule == NULL ||
        schedule->phase != UMI_BUILD_AUTOMATION_SCHEDULE_VERIFYING) {
        return UMI_STATUS_INVALID_STATE;
    }
    schedule->verification_completed_at_ms = completed_at_ms;
    if (passed) {
        schedule->verified_revision = schedule->change_revision;
        schedule->phase =
            UMI_BUILD_AUTOMATION_SCHEDULE_WAITING_FOR_BUILD;
        return UMI_STATUS_OK;
    }
    schedule->verified_revision = 0U;
    schedule->manual_requested = 0;
    schedule->failed_verifications += 1U;
    schedule->phase = UMI_BUILD_AUTOMATION_SCHEDULE_BLOCKED;
    return UMI_STATUS_PERMISSION_DENIED;
}

/* Enter compilation only for the unchanged revision that passed verification. */
UmiStatus umi_build_automation_schedule_begin_build(
    UmiBuildAutomationSchedule *schedule,
    uint64_t started_at_ms)
{
    if (schedule == NULL ||
        umi_build_automation_schedule_next_action(schedule, started_at_ms) !=
            UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_BUILD) {
        return UMI_STATUS_INVALID_STATE;
    }
    schedule->phase = UMI_BUILD_AUTOMATION_SCHEDULE_BUILDING;
    schedule->build_started_at_ms = started_at_ms;
    return UMI_STATUS_OK;
}

/* Finish the generation, retaining failed evidence for an explicit repair. */
UmiStatus umi_build_automation_schedule_complete_build(
    UmiBuildAutomationSchedule *schedule,
    int passed,
    uint64_t completed_at_ms)
{
    if (schedule == NULL ||
        schedule->phase != UMI_BUILD_AUTOMATION_SCHEDULE_BUILDING) {
        return UMI_STATUS_INVALID_STATE;
    }
    schedule->last_build_completed_at_ms = completed_at_ms;
    schedule->manual_requested = 0;
    if (passed) {
        schedule->successful_builds += 1U;
        schedule->pending_changes = 0;
        schedule->first_pending_change_at_ms = 0U;
        schedule->last_change_at_ms = 0U;
        schedule->verified_revision = 0U;
        schedule->phase = UMI_BUILD_AUTOMATION_SCHEDULE_IDLE;
        return UMI_STATUS_OK;
    }
    schedule->failed_builds += 1U;
    schedule->verified_revision = 0U;
    schedule->phase = UMI_BUILD_AUTOMATION_SCHEDULE_BLOCKED;
    return UMI_STATUS_INTERNAL_ERROR;
}

/* Translate schedule phases into stable status text. */
const char *umi_build_automation_schedule_phase_text(
    UmiBuildAutomationSchedulePhase phase)
{
    switch (phase) {
        case UMI_BUILD_AUTOMATION_SCHEDULE_IDLE: return "idle";
        case UMI_BUILD_AUTOMATION_SCHEDULE_WAITING_FOR_VERIFICATION:
            return "waiting-for-verification";
        case UMI_BUILD_AUTOMATION_SCHEDULE_VERIFYING: return "verifying";
        case UMI_BUILD_AUTOMATION_SCHEDULE_WAITING_FOR_BUILD:
            return "waiting-for-build";
        case UMI_BUILD_AUTOMATION_SCHEDULE_BUILDING: return "building";
        case UMI_BUILD_AUTOMATION_SCHEDULE_BLOCKED: return "blocked";
        default: return "unknown";
    }
}

/* Translate scheduler actions into stable status text. */
const char *umi_build_automation_schedule_action_text(
    UmiBuildAutomationScheduleAction action)
{
    switch (action) {
        case UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_NONE: return "none";
        case UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_VERIFY: return "verify";
        case UMI_BUILD_AUTOMATION_SCHEDULE_ACTION_BUILD: return "build";
        default: return "unknown";
    }
}
