/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/launch_selection.c
 *
 * PURPOSE:
 *   Implement safe multi-application selection over the existing governed
 *   application catalogue and launcher.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/launch_selection.h"

#include <stdlib.h>
#include <string.h>

struct UmiApplicationLaunchSelection {
    UmiApplicationRuntimeCatalogue *catalogue;
    UmiApplicationLaunchChoice choices[
        UMI_APPLICATION_RUNTIME_MAX_APPLICATIONS];
    size_t count;
    uint64_t revision;
};

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source,
                           bool allow_empty)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL || source[0] == '\0') {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (!allow_empty) return UMI_STATUS_INVALID_ARGUMENT;
        destination[0] = '\0';
        return UMI_STATUS_OK;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the find mutable operation used by this module and its client applications. */
static UmiApplicationLaunchChoice *find_mutable(
    UmiApplicationLaunchSelection *selection,
    const char *application_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || application_id == NULL ||
        application_id[0] == '\0') {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(selection->choices[index].application_id,
                   application_id) == 0) {
            return &selection->choices[index];
        }
    }
    return NULL;
}

/* Provide the find const operation used by this module and its client applications. */
static const UmiApplicationLaunchChoice *find_const(
    const UmiApplicationLaunchSelection *selection,
    const char *application_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || application_id == NULL ||
        application_id[0] == '\0') {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(selection->choices[index].application_id,
                   application_id) == 0) {
            return &selection->choices[index];
        }
    }
    return NULL;
}

/*
 * Provide the previous selection operation used by this module and its client
 * applications.
 */
static bool previous_selection(
    const UmiApplicationLaunchChoice *choices,
    size_t count,
    const char *application_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(choices[index].application_id, application_id) == 0) {
            return choices[index].selected;
        }
    }
    return false;
}

/*
 * Initialise application launch selection from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_launch_selection_create(
    UmiApplicationRuntimeCatalogue *catalogue,
    UmiApplicationLaunchSelection **out_selection)
{
    UmiApplicationLaunchSelection *selection;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || out_selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_selection = NULL;
    selection = (UmiApplicationLaunchSelection *)calloc(
        1U, sizeof(*selection));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    selection->catalogue = catalogue;
    selection->revision = 1U;
    status = umi_application_launch_selection_refresh(selection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(selection);
        return status;
    }
    *out_selection = selection;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by application launch selection so the same storage can be
 * reused safely.
 */
void umi_application_launch_selection_destroy(
    UmiApplicationLaunchSelection *selection)
{
    free(selection);
}

/*
 * Provide the application launch selection refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_launch_selection_refresh(
    UmiApplicationLaunchSelection *selection)
{
    UmiApplicationLaunchChoice previous[
        UMI_APPLICATION_RUNTIME_MAX_APPLICATIONS];
    const size_t catalogue_count = selection != NULL
        ? umi_application_runtime_catalogue_count(selection->catalogue)
        : 0U;
    const size_t previous_count = selection != NULL
        ? selection->count
        : 0U;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Preserve explicit user choices while refreshing mutable runtime state. */
    (void)memcpy(previous, selection->choices, sizeof(previous));
    selection->count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue_count; ++index) {
        UmiApplicationRuntimeRecord record;
        UmiApplicationLaunchChoice *choice;
        UmiStatus status = umi_application_runtime_catalogue_at(
            selection->catalogue, index, &record);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        /* Umicom Desk is the host, so it must not offer to launch itself. */
        if (strcmp(record.application_id, "org.umicom.desktop") == 0) {
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (selection->count >= UMI_APPLICATION_RUNTIME_MAX_APPLICATIONS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        choice = &selection->choices[selection->count];
        (void)memset(choice, 0, sizeof(*choice));
        status = copy_text(choice->application_id,
                           sizeof(choice->application_id),
                           record.application_id, false);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = copy_text(choice->display_name,
                               sizeof(choice->display_name),
                               record.display_name, false);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = copy_text(choice->icon_resource_id,
                               sizeof(choice->icon_resource_id),
                               record.icon_resource_id, true);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        choice->state = record.state;
        choice->eligible = record.installed && record.compatible &&
                           record.enabled && record.visible;
        choice->running = record.running;
        choice->selected = choice->eligible && previous_selection(
            previous, previous_count, record.application_id);
        choice->revision = record.revision;
        selection->count += 1U;
    }
    selection->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find application launch selection set while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_application_launch_selection_set_selected(
    UmiApplicationLaunchSelection *selection,
    const char *application_id,
    bool selected)
{
    UmiApplicationLaunchChoice *choice = find_mutable(
        selection, application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (choice == NULL) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (selected && !choice->eligible) return UMI_STATUS_UNAVAILABLE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (choice->selected != selected) {
        choice->selected = selected;
        selection->revision += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the application launch selection select all operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_launch_selection_select_all(
    UmiApplicationLaunchSelection *selection)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->count; ++index) {
        selection->choices[index].selected =
            selection->choices[index].eligible;
    }
    selection->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by application launch selection so the same storage can be
 * reused safely.
 */
UmiStatus umi_application_launch_selection_clear(
    UmiApplicationLaunchSelection *selection)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->count; ++index) {
        selection->choices[index].selected = false;
    }
    selection->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find application launch selection while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_application_launch_selection_at(
    const UmiApplicationLaunchSelection *selection,
    size_t index,
    UmiApplicationLaunchChoice *out_choice)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || out_choice == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= selection->count) return UMI_STATUS_NOT_FOUND;
    *out_choice = selection->choices[index];
    return UMI_STATUS_OK;
}

/*
 * Find application launch selection while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_application_launch_selection_find(
    const UmiApplicationLaunchSelection *selection,
    const char *application_id,
    UmiApplicationLaunchChoice *out_choice)
{
    const UmiApplicationLaunchChoice *choice;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_choice == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    choice = find_const(selection, application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (choice == NULL) return UMI_STATUS_NOT_FOUND;
    *out_choice = *choice;
    return UMI_STATUS_OK;
}

/*
 * Provide the application launch selection snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_launch_selection_snapshot(
    const UmiApplicationLaunchSelection *selection,
    UmiApplicationLaunchSelectionSnapshot *out_snapshot)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->choice_count = selection->count;
    out_snapshot->revision = selection->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->count; ++index) {
        const UmiApplicationLaunchChoice *choice =
            &selection->choices[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (choice->eligible) out_snapshot->eligible_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (choice->selected) out_snapshot->selected_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (choice->running) out_snapshot->running_count += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Perform application launch selection through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_application_launch_selection_execute(
    UmiApplicationLaunchSelection *selection,
    UmiApplicationLauncher *launcher,
    UmiApplicationLaunchSelectionReport *out_report)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || launcher == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->first_failure = UMI_STATUS_OK;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->count; ++index) {
        UmiApplicationLaunchChoice *choice = &selection->choices[index];
        UmiApplicationLaunchResult *result;
        UmiApplicationLauncherSnapshot launcher_snapshot;
        UmiStatus status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!choice->selected) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (out_report->result_count >=
            UMI_APPLICATION_LAUNCH_SELECTION_MAX_RESULTS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        result = &out_report->results[out_report->result_count];
        (void)memset(result, 0, sizeof(*result));
        (void)copy_text(result->application_id,
                        sizeof(result->application_id),
                        choice->application_id, false);

        /* The existing launcher resolves a running product to ACTIVATE and
         * records both preparation and adapter failures in its audit state. */
        result->resolved_action = choice->running
            ? UMI_APPLICATION_LAUNCH_ACTIVATE
            : UMI_APPLICATION_LAUNCH_START;
        status = umi_application_launcher_request(
            launcher, choice->application_id,
            UMI_APPLICATION_LAUNCH_START);
        /* A running application is resolved to ACTIVATE inside the governed
         * launcher. Read back the executed plan so the report reflects that
         * authoritative decision even if runtime state changed meanwhile. */
        if (status == UMI_STATUS_OK &&
            umi_application_launcher_snapshot(
                launcher, &launcher_snapshot) == UMI_STATUS_OK &&
            strcmp(launcher_snapshot.last_plan.application_id,
                   choice->application_id) == 0) {
            result->resolved_action =
                launcher_snapshot.last_plan.action;
        }
        result->status = status;
        out_report->result_count += 1U;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (result->resolved_action ==
                UMI_APPLICATION_LAUNCH_ACTIVATE) {
                out_report->activated_count += 1U;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                out_report->started_count += 1U;
            }
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            out_report->failed_count += 1U;
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (out_report->first_failure == UMI_STATUS_OK) {
                out_report->first_failure = status;
            }
        }
    }
    selection->revision += 1U;
    {
        UmiStatus refresh_status =
            umi_application_launch_selection_refresh(selection);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (out_report->first_failure == UMI_STATUS_OK &&
            refresh_status != UMI_STATUS_OK) {
            out_report->first_failure = refresh_status;
        }
    }
    out_report->revision = selection->revision;
    return out_report->first_failure;
}
