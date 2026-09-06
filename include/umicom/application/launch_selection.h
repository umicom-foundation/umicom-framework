/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/launch_selection.h
 *
 * PURPOSE:
 *   Define a toolkit-neutral application picker that can start or activate
 *   several installed Umicom applications from one user action.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_LAUNCH_SELECTION_H
#define UMICOM_APPLICATION_LAUNCH_SELECTION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/application/launcher.h"
#include "umicom/application/runtime/readiness.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_LAUNCH_SELECTION_MAX_RESULTS \
    UMI_APPLICATION_RUNTIME_MAX_APPLICATIONS

/**
 * Represent the application launch choice data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationLaunchChoice {
    char application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    char display_name[UMI_APPLICATION_RUNTIME_NAME_CAPACITY];
    char icon_resource_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    /* Default layout is retained so a restored selection can explain its starting surface. */
    char layout_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    UmiApplicationRuntimeState state;
    bool selected;
    bool eligible;
    bool running;
    uint64_t revision;
    /* The shared gate explains whether the Framework workspace can be opened. */
    UmiApplicationLaunchReadinessState readiness_state;
    /* Feature maturity remains informational and does not by itself block launch. */
    unsigned readiness_percent;
    /* A bounded human-readable explanation is safe for GUI labels and logs. */
    char readiness_reason[UMI_APPLICATION_RUNTIME_MESSAGE_CAPACITY];
} UmiApplicationLaunchChoice;

/**
 * Represent the application launch result data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationLaunchResult {
    char application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    UmiApplicationLaunchAction resolved_action;
    UmiStatus status;
} UmiApplicationLaunchResult;

/**
 * Represent the application launch selection report data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationLaunchSelectionReport {
    UmiApplicationLaunchResult results[
        UMI_APPLICATION_LAUNCH_SELECTION_MAX_RESULTS];
    size_t result_count;
    size_t started_count;
    size_t activated_count;
    size_t failed_count;
    UmiStatus first_failure;
    uint64_t revision;
} UmiApplicationLaunchSelectionReport;

/**
 * Represent the application launch selection snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationLaunchSelectionSnapshot {
    size_t choice_count;
    size_t eligible_count;
    size_t selected_count;
    size_t running_count;
    /* Number of choices blocked because their Framework experience is incomplete. */
    size_t readiness_blocked_count;
    uint64_t revision;
} UmiApplicationLaunchSelectionSnapshot;

/**
 * Carry a bounded multi-application selection between sessions or storage
 * providers without exposing the launch-selection implementation.
 */
typedef struct UmiApplicationLaunchSelectionCheckpoint {
    uint32_t structure_size;
    char application_ids[UMI_APPLICATION_LAUNCH_SELECTION_MAX_RESULTS]
                         [UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    char layout_ids[UMI_APPLICATION_LAUNCH_SELECTION_MAX_RESULTS]
                   [UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    size_t selected_count;
    uint64_t source_revision;
} UmiApplicationLaunchSelectionCheckpoint;

/**
 * Represent the application launch selection data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationLaunchSelection UmiApplicationLaunchSelection;

/**
 * Ask the active host to open one application. The ID is borrowed until this
 * call returns; a host that queues work must copy it. OK means that the host
 * accepted the request, not that a process started or a workspace is ready.
 * The caller owns context and keeps it alive throughout the dispatch.
 */
typedef UmiStatus (*UmiApplicationLaunchDispatchFn)(
    const char *application_id, void *context);

/** Record one host decision without inventing a running process or launch action. */
typedef struct UmiApplicationLaunchDispatchResult {
    char application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    UmiStatus status;
} UmiApplicationLaunchDispatchResult;

/**
 * Describe every selected request in catalogue order. These copied values are
 * caller-owned and remain valid after the selection is changed or destroyed.
 */
typedef struct UmiApplicationLaunchDispatchReport {
    UmiApplicationLaunchDispatchResult results[
        UMI_APPLICATION_LAUNCH_SELECTION_MAX_RESULTS];
    size_t result_count;
    size_t accepted_count;
    size_t failed_count;
    UmiStatus first_failure;
    uint64_t revision;
} UmiApplicationLaunchDispatchReport;

/**
 * Initialise application launch selection from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_launch_selection_create(
    UmiApplicationRuntimeCatalogue *catalogue,
    UmiApplicationLaunchSelection **out_selection);
/**
 * Create a picker for any host application, hiding only that host's own entry.
 * A nonempty host ID must exist in the borrowed catalogue; NULL or an empty ID
 * includes every product. The older create operation continues to hide Desk.
 * The catalogue must outlive the selection. Calls use one owner thread.
 * Explicit selections survive a temporary loss of eligibility during refresh,
 * so failed requests remain visible for retry when the product is available.
 */
UmiStatus umi_application_launch_selection_create_for_host(
    UmiApplicationRuntimeCatalogue *catalogue,
    const char *host_application_id,
    UmiApplicationLaunchSelection **out_selection);
/**
 * Release or reset state held by application launch selection so the same storage can be
 * reused safely.
 */
void umi_application_launch_selection_destroy(
    UmiApplicationLaunchSelection *selection);

/**
 * Provide the application launch selection refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_launch_selection_refresh(
    UmiApplicationLaunchSelection *selection);
/**
 * Find application launch selection set while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_application_launch_selection_set_selected(
    UmiApplicationLaunchSelection *selection,
    const char *application_id,
    bool selected);
/**
 * Provide the application launch selection select all operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_launch_selection_select_all(
    UmiApplicationLaunchSelection *selection);
/**
 * Release or reset state held by application launch selection so the same storage can be
 * reused safely.
 */
UmiStatus umi_application_launch_selection_clear(
    UmiApplicationLaunchSelection *selection);
/**
 * Find application launch selection while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_application_launch_selection_at(
    const UmiApplicationLaunchSelection *selection,
    size_t index,
    UmiApplicationLaunchChoice *out_choice);
/**
 * Find application launch selection while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_application_launch_selection_find(
    const UmiApplicationLaunchSelection *selection,
    const char *application_id,
    UmiApplicationLaunchChoice *out_choice);
/**
 * Provide the application launch selection snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_launch_selection_snapshot(
    const UmiApplicationLaunchSelection *selection,
    UmiApplicationLaunchSelectionSnapshot *out_snapshot);

/**
 * Capture selected applications and their default layouts for later session
 * restore. The caller owns the returned value and may store it in its chosen
 * Framework persistence service.
 */
UmiStatus umi_application_launch_selection_checkpoint_capture(
    const UmiApplicationLaunchSelection *selection,
    UmiApplicationLaunchSelectionCheckpoint *out_checkpoint);

/**
 * Restore a previously captured selection after validating every application
 * still exists and is currently launchable.
 */
UmiStatus umi_application_launch_selection_checkpoint_restore(
    UmiApplicationLaunchSelection *selection,
    const UmiApplicationLaunchSelectionCheckpoint *checkpoint);

/*
 * Every selected application is attempted even when an earlier application
 * fails. This lets the caller show a useful per-application result instead of
 * leaving the user unsure which programs started.
 */
UmiStatus umi_application_launch_selection_execute(
    UmiApplicationLaunchSelection *selection,
    UmiApplicationLauncher *launcher,
    UmiApplicationLaunchSelectionReport *out_report);

/**
 * Dispatch selected requests to a host-owned opener on the calling thread.
 * Each success clears that selection so Retry cannot open it again; failures
 * remain selected. Every selected item is reported, including CANCELLED, and
 * later items are still attempted. An empty selection returns INVALID_STATE
 * with an empty report. The first failed request is the overall return value.
 *
 * This model is single-threaded. Callbacks may inspect copied choices but may
 * not mutate, refresh, execute or recursively dispatch the selection: these
 * operations return BUSY until dispatch finishes. Neither the selection nor
 * its borrowed catalogue/context may be destroyed inside a callback. No
 * runtime state or process token is changed by an accepted host request.
 */
UmiStatus umi_application_launch_selection_dispatch(
    UmiApplicationLaunchSelection *selection,
    UmiApplicationLaunchDispatchFn callback,
    void *context,
    UmiApplicationLaunchDispatchReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
