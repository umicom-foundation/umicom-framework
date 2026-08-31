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

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_LAUNCH_SELECTION_MAX_RESULTS \
    UMI_APPLICATION_RUNTIME_MAX_APPLICATIONS

typedef struct UmiApplicationLaunchChoice {
    char application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    char display_name[UMI_APPLICATION_RUNTIME_NAME_CAPACITY];
    char icon_resource_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    UmiApplicationRuntimeState state;
    bool selected;
    bool eligible;
    bool running;
    uint64_t revision;
} UmiApplicationLaunchChoice;

typedef struct UmiApplicationLaunchResult {
    char application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    UmiApplicationLaunchAction resolved_action;
    UmiStatus status;
} UmiApplicationLaunchResult;

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

typedef struct UmiApplicationLaunchSelectionSnapshot {
    size_t choice_count;
    size_t eligible_count;
    size_t selected_count;
    size_t running_count;
    uint64_t revision;
} UmiApplicationLaunchSelectionSnapshot;

typedef struct UmiApplicationLaunchSelection UmiApplicationLaunchSelection;

UmiStatus umi_application_launch_selection_create(
    UmiApplicationRuntimeCatalogue *catalogue,
    UmiApplicationLaunchSelection **out_selection);
void umi_application_launch_selection_destroy(
    UmiApplicationLaunchSelection *selection);

UmiStatus umi_application_launch_selection_refresh(
    UmiApplicationLaunchSelection *selection);
UmiStatus umi_application_launch_selection_set_selected(
    UmiApplicationLaunchSelection *selection,
    const char *application_id,
    bool selected);
UmiStatus umi_application_launch_selection_select_all(
    UmiApplicationLaunchSelection *selection);
UmiStatus umi_application_launch_selection_clear(
    UmiApplicationLaunchSelection *selection);
UmiStatus umi_application_launch_selection_at(
    const UmiApplicationLaunchSelection *selection,
    size_t index,
    UmiApplicationLaunchChoice *out_choice);
UmiStatus umi_application_launch_selection_find(
    const UmiApplicationLaunchSelection *selection,
    const char *application_id,
    UmiApplicationLaunchChoice *out_choice);
UmiStatus umi_application_launch_selection_snapshot(
    const UmiApplicationLaunchSelection *selection,
    UmiApplicationLaunchSelectionSnapshot *out_snapshot);

/*
 * Every selected application is attempted even when an earlier application
 * fails. This lets the caller show a useful per-application result instead of
 * leaving the user unsure which programs started.
 */
UmiStatus umi_application_launch_selection_execute(
    UmiApplicationLaunchSelection *selection,
    UmiApplicationLauncher *launcher,
    UmiApplicationLaunchSelectionReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
