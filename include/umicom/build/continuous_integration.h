/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/continuous_integration.h
 *
 * PURPOSE:
 *   Turn file-change events into debounced build generations and publish
 *   versioned module-update notices after successful build and test work.
 *
 * ARCHITECTURE:
 *   The service owns planning state but does not own a filesystem thread or
 *   execute a compiler. A command, Studio, Desk or another application feeds
 *   it watcher events, executes the returned plan with the Framework build
 *   service, and reports completion. This keeps policy reusable and makes the
 *   state machine deterministic in tests.
 *
 * OWNERSHIP AND THREADING:
 *   Registered scopes and changed paths are copied. Returned plan items and
 *   snapshots are values. The optional update sink remains caller-owned and is
 *   called synchronously. A host must serialise access when several threads
 *   feed the same service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_CONTINUOUS_INTEGRATION_H
#define UMICOM_BUILD_CONTINUOUS_INTEGRATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/build/automation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_CONTINUOUS_API_VERSION 1U
#define UMI_BUILD_CONTINUOUS_UPDATE_TOPIC \
    "umicom.build.module-update.available"

/* These phases describe one local continuous-integration generation. */
typedef enum UmiBuildContinuousPhase {
    UMI_BUILD_CONTINUOUS_IDLE = 0,
    UMI_BUILD_CONTINUOUS_DEBOUNCING = 1,
    UMI_BUILD_CONTINUOUS_RUNNING = 2,
    UMI_BUILD_CONTINUOUS_SUCCEEDED = 3,
    UMI_BUILD_CONTINUOUS_FAILED = 4
} UmiBuildContinuousPhase;

/*
 * A successful notice tells running applications that a newly built module is
 * available. Native code is marked restart_required because replacing code
 * that is executing could leave live pointers referring to an old image.
 */
typedef struct UmiBuildContinuousUpdate {
    uint32_t structure_size;
    uint32_t api_version;
    char scope_id[UMI_BUILD_ID_CAPACITY];
    char display_name[UMI_BUILD_NAME_CAPACITY];
    char version[UMI_BUILD_NAME_CAPACITY];
    char build_target[UMI_BUILD_NAME_CAPACITY];
    uint64_t generation;
    uint64_t available_at_ms;
    int restart_required;
} UmiBuildContinuousUpdate;

/* Return a status so durable publication can be required by policy. */
typedef UmiStatus (*UmiBuildContinuousUpdateSink)(
    const UmiBuildContinuousUpdate *update,
    void *user_data);

/* Configuration is copied; callback user data remains caller-owned. */
typedef struct UmiBuildContinuousConfig {
    uint32_t structure_size;
    uint32_t api_version;
    uint32_t debounce_ms;
    /* Continue after the last durable generation when a host restarts. */
    uint64_t initial_generation;
    UmiBuildContinuousUpdateSink update_sink;
    void *update_user_data;
    int require_update_record;
} UmiBuildContinuousConfig;

/* A copied snapshot supports status bars, dashboards and command output. */
typedef struct UmiBuildContinuousSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    UmiBuildContinuousPhase phase;
    size_t queued_event_count;
    size_t batch_change_count;
    size_t batch_scope_count;
    uint64_t generation;
    uint64_t successful_generations;
    uint64_t failed_generations;
    uint64_t published_updates;
    uint64_t last_change_at_ms;
    uint64_t last_started_at_ms;
    uint64_t last_completed_at_ms;
    UmiStatus last_status;
    uint64_t revision;
} UmiBuildContinuousSnapshot;

typedef struct UmiBuildContinuousIntegration UmiBuildContinuousIntegration;

/* Return conservative defaults with a short editor-save debounce window. */
UmiBuildContinuousConfig umi_build_continuous_config_default(void);

/* Allocate a service and its owned affected-scope planner. */
UmiStatus umi_build_continuous_create(
    const UmiBuildContinuousConfig *config,
    UmiBuildContinuousIntegration **out_service);

/* Release the planner and service without touching callback-owned state. */
void umi_build_continuous_destroy(UmiBuildContinuousIntegration *service);

/* Return the borrowed planner so a host can register discovered scopes. */
UmiBuildAutomation *umi_build_continuous_planner(
    UmiBuildContinuousIntegration *service);

/* Queue one watcher event and restart the debounce window. */
UmiStatus umi_build_continuous_record_change(
    UmiBuildContinuousIntegration *service,
    const char *workspace_relative_path,
    int deleted,
    uint64_t observed_at_ms);

/* Return non-zero when the quiet period has elapsed and a batch can begin. */
int umi_build_continuous_ready(
    const UmiBuildContinuousIntegration *service,
    uint64_t now_ms);

/* Evaluate and freeze the current batch for execution by the host. */
UmiStatus umi_build_continuous_begin(
    UmiBuildContinuousIntegration *service,
    uint64_t started_at_ms);

/* Return the number of affected scopes in the active frozen batch. */
size_t umi_build_continuous_plan_count(
    const UmiBuildContinuousIntegration *service);

/* Copy one active plan item without exposing service-owned storage. */
UmiStatus umi_build_continuous_plan_item_at(
    const UmiBuildContinuousIntegration *service,
    size_t position,
    UmiBuildAutomationPlanItem *out_item);

/*
 * Finish the active batch. Successful builds publish update notices and clear
 * their evidence. Failed evidence is retained for the next edited batch. The
 * completion value uses the debounce clock; availability uses wall-clock time.
 */
UmiStatus umi_build_continuous_complete(
    UmiBuildContinuousIntegration *service,
    int build_and_tests_succeeded,
    uint64_t completed_at_ms,
    uint64_t available_at_ms);

/* Copy observable lifecycle counters without exposing internal storage. */
UmiStatus umi_build_continuous_snapshot(
    const UmiBuildContinuousIntegration *service,
    UmiBuildContinuousSnapshot *out_snapshot);

/* Format a notice as bounded JSON suitable for the Framework change broker. */
UmiStatus umi_build_continuous_update_format_json(
    const UmiBuildContinuousUpdate *update,
    char *out_json,
    size_t capacity);

/* Return stable display text for one lifecycle phase. */
const char *umi_build_continuous_phase_text(UmiBuildContinuousPhase phase);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_BUILD_CONTINUOUS_INTEGRATION_H */
