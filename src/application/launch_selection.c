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
    /* Only the active host is hidden; another application may still open Desk. */
    char excluded_application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    /* Shared pickers retain failed requests through a temporary outage.
     * The original Desk constructor keeps its established refresh behaviour. */
    bool retain_unavailable_selection;
    UmiApplicationLaunchChoice choices[
        UMI_APPLICATION_RUNTIME_MAX_APPLICATIONS];
    size_t count;
    uint64_t revision;
    /* A callback may inspect choices, but cannot change the active iteration. */
    bool dispatching;
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

/* Confirm a fixed-width checkpoint field has a terminator before string APIs read it. */
static UmiStatus checkpoint_text_validate(const char *text,
                                          size_t capacity,
                                          bool allow_empty)
{
    /* Reject missing storage or a field that is not terminated inside its contract. */
    if (text == NULL || capacity == 0U || memchr(text, '\0', capacity) == NULL) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* An empty application ID cannot identify a record during restore. */
    if (!allow_empty && text[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
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
static UmiStatus create_selection(
    UmiApplicationRuntimeCatalogue *catalogue,
    const char *excluded_application_id,
    bool retain_unavailable_selection,
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
    selection->retain_unavailable_selection = retain_unavailable_selection;
    selection->revision = 1U;
    status = copy_text(selection->excluded_application_id,
                       sizeof(selection->excluded_application_id),
                       excluded_application_id, true);
    /* Invalid or overlong host IDs never produce a partially usable picker. */
    if (status == UMI_STATUS_OK) {
        status = umi_application_launch_selection_refresh(selection);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(selection);
        return status;
    }
    *out_selection = selection;
    return UMI_STATUS_OK;
}

/* Preserve the original Desk-specific constructor for existing callers. */
UmiStatus umi_application_launch_selection_create(
    UmiApplicationRuntimeCatalogue *catalogue,
    UmiApplicationLaunchSelection **out_selection)
{
    return create_selection(catalogue, "org.umicom.desktop", false, out_selection);
}

/* A shared shell identifies its own host instead of always hiding Desk. */
UmiStatus umi_application_launch_selection_create_for_host(
    UmiApplicationRuntimeCatalogue *catalogue,
    const char *host_application_id,
    UmiApplicationLaunchSelection **out_selection)
{
    UmiApplicationRuntimeRecord record;
    UmiStatus status;
    /* Reject unknown hosts without handing the caller an uninitialised owner. */
    if (out_selection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_selection = NULL;
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (host_application_id != NULL && host_application_id[0] != '\0') {
        if (strlen(host_application_id) >= UMI_APPLICATION_RUNTIME_ID_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = umi_application_runtime_catalogue_find(
            catalogue, host_application_id, &record);
        if (status != UMI_STATUS_OK) return status;
    }
    return create_selection(catalogue, host_application_id, true, out_selection);
}

/*
 * Release or reset state held by application launch selection so the same storage can be
 * reused safely.
 */
void umi_application_launch_selection_destroy(
    UmiApplicationLaunchSelection *selection)
{
    /* Destruction during a host callback would invalidate the active iterator.
     * The owner must call destroy again after dispatch has returned. */
    if (selection != NULL && selection->dispatching) return;
    free(selection);
}

/* Bind explicit outage-retention policy without changing current selections. */
UmiStatus umi_application_launch_selection_set_retention(
    UmiApplicationLaunchSelection *selection, bool retain_unavailable)
{
    if (selection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (selection->dispatching) return UMI_STATUS_BUSY;
    selection->retain_unavailable_selection = retain_unavailable;
    return UMI_STATUS_OK;
}

/*
 * Provide the application launch selection refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_launch_selection_refresh(
    UmiApplicationLaunchSelection *selection)
{
    UmiApplicationLaunchSelection *next;
    const size_t catalogue_count = selection != NULL
        ? umi_application_runtime_catalogue_count(selection->catalogue)
        : 0U;
    const size_t previous_count = selection != NULL
        ? selection->count
        : 0U;
    size_t index;
    UmiStatus refresh_status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* A host callback cannot replace the choices being dispatched. */
    if (selection->dispatching) return UMI_STATUS_BUSY;

    /* Preserve explicit user choices while refreshing mutable runtime state.
     * Build a replacement on the heap: errors leave the old snapshot intact,
     * and a GUI callback does not need a large fixed array on its stack. */
    next = (UmiApplicationLaunchSelection *)calloc(1U, sizeof(*next));
    if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue_count; ++index) {
        UmiApplicationRuntimeRecord record;
        UmiApplicationLaunchReadiness readiness;
        UmiApplicationLaunchChoice *choice;
        UmiStatus status = umi_application_runtime_catalogue_at(
            selection->catalogue, index, &record);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            refresh_status = status;
            goto finish_refresh;
        }

        /* The host must not offer to launch itself. Legacy Desk callers still
         * use Desk here, while other shells name their own application. */
        if (strcmp(record.application_id, selection->excluded_application_id) == 0) {
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (next->count >= UMI_APPLICATION_RUNTIME_MAX_APPLICATIONS) {
            refresh_status = UMI_STATUS_CAPACITY_EXCEEDED;
            goto finish_refresh;
        }
        choice = &next->choices[next->count];
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
        /* Retain the registered default layout beside the choice for session checkpoints. */
        if (status == UMI_STATUS_OK) {
            status = copy_text(choice->layout_id,
                               sizeof(choice->layout_id),
                               record.default_layout_id, true);
        }
        /* Check the shared Framework experience before offering this product to the user. */
        if (status == UMI_STATUS_OK) {
            status = umi_application_launch_readiness_check(
                record.application_id, &readiness);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            refresh_status = status;
            goto finish_refresh;
        }
        choice->state = record.state;
        choice->readiness_state = readiness.state;
        choice->readiness_percent = readiness.feature_readiness_percent;
        status = copy_text(choice->readiness_reason,
                           sizeof(choice->readiness_reason),
                           readiness.reason, true);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            refresh_status = status;
            goto finish_refresh;
        }
        choice->eligible = (record.installed || record.running) && record.compatible &&
                           record.enabled && record.visible &&
                           readiness.launchable;
        choice->running = record.running;
        /* A failed request stays checked in a shared picker even if Refresh
         * discovers an outage. Dispatch still checks current eligibility; the
         * retained flag records user intent, not permission to bypass policy. */
        choice->selected = (choice->eligible || selection->retain_unavailable_selection) &&
            previous_selection(selection->choices, previous_count, record.application_id);
        choice->revision = record.revision;
        next->count += 1U;
    }
    /* Publish a complete replacement only after every entry was validated. */
    (void)memcpy(selection->choices, next->choices, sizeof(selection->choices));
    selection->count = next->count;
    selection->revision += 1U;
finish_refresh:
    free(next);
    return refresh_status;
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
    UmiApplicationLaunchChoice *choice;
    /* Validate the owner before lookup, and keep an active dispatch stable. */
    if (selection == NULL || application_id == NULL || application_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (selection->dispatching) return UMI_STATUS_BUSY;
    choice = find_mutable(selection, application_id);
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
    /* Reentrant callbacks cannot add work to the currently dispatched set. */
    if (selection->dispatching) return UMI_STATUS_BUSY;
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
    /* Keep per-request reporting stable while a host callback is active. */
    if (selection->dispatching) return UMI_STATUS_BUSY;
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
        /* Keep blocked choices visible to the caller so the picker can explain them. */
        if (choice->readiness_state != UMI_APPLICATION_LAUNCH_READINESS_READY) {
            out_snapshot->readiness_blocked_count += 1U;
        }
    }
    return UMI_STATUS_OK;
}

/* Capture selected choices as bounded IDs so a caller can persist a multi-app session. */
UmiStatus umi_application_launch_selection_checkpoint_capture(
    const UmiApplicationLaunchSelection *selection,
    UmiApplicationLaunchSelectionCheckpoint *out_checkpoint)
{
    size_t index;

    /* Reject missing state before reading the selection or writing the checkpoint. */
    if (selection == NULL || out_checkpoint == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_checkpoint, 0, sizeof(*out_checkpoint));
    out_checkpoint->structure_size = (uint32_t)sizeof(*out_checkpoint);
    out_checkpoint->source_revision = selection->revision;
    /* Copy only selected choices; unselected products need no session storage. */
    for (index = 0U; index < selection->count; ++index) {
        const UmiApplicationLaunchChoice *choice = &selection->choices[index];
        UmiApplicationRuntimeRecord record;
        UmiStatus status;

        /* Keep the operation inside its fixed checkpoint capacity. */
        if (!choice->selected) continue;
        if (out_checkpoint->selected_count >=
            UMI_APPLICATION_LAUNCH_SELECTION_MAX_RESULTS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        /* A selected choice must remain eligible; otherwise its state is inconsistent. */
        if (!choice->eligible) return UMI_STATUS_UNAVAILABLE;
        status = umi_application_runtime_catalogue_find(
            selection->catalogue, choice->application_id, &record);
        /* Preserve the catalogue failure so the checkpoint cannot hide stale state. */
        if (status != UMI_STATUS_OK) return status;
        status = copy_text(
            out_checkpoint->application_ids[out_checkpoint->selected_count],
            sizeof(out_checkpoint->application_ids[out_checkpoint->selected_count]),
            choice->application_id,
            false);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = copy_text(
            out_checkpoint->layout_ids[out_checkpoint->selected_count],
            sizeof(out_checkpoint->layout_ids[out_checkpoint->selected_count]),
            record.default_layout_id,
            true);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        out_checkpoint->selected_count += 1U;
    }
    return UMI_STATUS_OK;
}

/* Restore a checkpoint atomically after every saved application passes current launch rules. */
UmiStatus umi_application_launch_selection_checkpoint_restore(
    UmiApplicationLaunchSelection *selection,
    const UmiApplicationLaunchSelectionCheckpoint *checkpoint)
{
    bool previous_selected[UMI_APPLICATION_RUNTIME_MAX_APPLICATIONS];
    size_t index;
    uint64_t original_revision;

    /* Reject missing state or an incompatible checkpoint layout before reading its arrays. */
    if (selection == NULL || checkpoint == NULL ||
        checkpoint->structure_size != sizeof(*checkpoint) ||
        checkpoint->selected_count > UMI_APPLICATION_LAUNCH_SELECTION_MAX_RESULTS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Restoring replaces the request set and must wait until dispatch ends. */
    if (selection->dispatching) return UMI_STATUS_BUSY;
    /* Validate all IDs and detect duplicates before changing the live selection. */
    for (index = 0U; index < checkpoint->selected_count; ++index) {
        const UmiApplicationLaunchChoice *choice;
        UmiStatus status = checkpoint_text_validate(
            checkpoint->application_ids[index],
            sizeof(checkpoint->application_ids[index]),
            false);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = checkpoint_text_validate(
            checkpoint->layout_ids[index],
            sizeof(checkpoint->layout_ids[index]),
            true);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        choice = find_const(selection, checkpoint->application_ids[index]);
        /* A removed product cannot be silently restored as a different product. */
        if (choice == NULL) return UMI_STATUS_NOT_FOUND;
        /* A saved selection is actionable only when the current gate allows it. */
        if (!choice->eligible) return UMI_STATUS_UNAVAILABLE;
        /* Duplicate IDs would make the restored order ambiguous. */
        for (size_t nested = 0U; nested < index; ++nested) {
            if (strcmp(checkpoint->application_ids[nested],
                       checkpoint->application_ids[index]) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }

    /* Save the old flags so a defensive rollback can keep the operation atomic. */
    original_revision = selection->revision;
    for (index = 0U; index < selection->count; ++index) {
        previous_selected[index] = selection->choices[index].selected;
    }
    (void)umi_application_launch_selection_clear(selection);
    /* Apply each validated ID; this loop cannot fail unless the catalogue changed concurrently. */
    for (index = 0U; index < checkpoint->selected_count; ++index) {
        UmiStatus status = umi_application_launch_selection_set_selected(
            selection, checkpoint->application_ids[index], true);
        if (status != UMI_STATUS_OK) {
            for (size_t nested = 0U; nested < selection->count; ++nested) {
                selection->choices[nested].selected = previous_selected[nested];
            }
            selection->revision = original_revision;
            return status;
        }
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
    /* A host callback must not start another execution over these choices. */
    if (selection->dispatching) return UMI_STATUS_BUSY;
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

/* Recheck current catalogue policy immediately before asking a host to open.
 * A previous callback may have changed availability of a later application. */
static UmiStatus dispatch_eligibility(
    const UmiApplicationLaunchSelection *selection,
    const UmiApplicationLaunchChoice *choice)
{
    UmiApplicationRuntimeRecord record;
    UmiApplicationLaunchReadiness readiness;
    UmiStatus status;
    /* A stale selection cannot bypass either the saved or current gate. */
    if (!choice->eligible) return UMI_STATUS_UNAVAILABLE;
    status = umi_application_runtime_catalogue_find(
        selection->catalogue, choice->application_id, &record);
    if (status != UMI_STATUS_OK) return status;
    if ((!record.installed && !record.running) || !record.compatible || !record.enabled || !record.visible) {
        return UMI_STATUS_UNAVAILABLE;
    }
    status = umi_application_launch_readiness_check(
        choice->application_id, &readiness);
    if (status != UMI_STATUS_OK) return status;
    return readiness.launchable ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
}

/* Keep request selection separate from process ownership. The host may accept
 * asynchronous work without having a process token or a ready window yet. */
UmiStatus umi_application_launch_selection_dispatch(
    UmiApplicationLaunchSelection *selection,
    UmiApplicationLaunchDispatchFn callback,
    void *context,
    UmiApplicationLaunchDispatchReport *out_report)
{
    size_t index;
    /* A nested call may reuse the outer report pointer. Do not clear that
     * caller-owned evidence before refusing the reentrant operation. */
    if (selection != NULL && selection->dispatching) return UMI_STATUS_BUSY;
    if (out_report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_report, 0, sizeof(*out_report));
    if (selection == NULL || callback == NULL) {
        out_report->first_failure = UMI_STATUS_INVALID_ARGUMENT;
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_report->revision = selection->revision;
    /* Validate capacity before invoking any callback, so no accepted request
     * can disappear from a partial report. Internal choices have the same bound. */
    if (selection->count > UMI_APPLICATION_LAUNCH_SELECTION_MAX_RESULTS) {
        out_report->first_failure = UMI_STATUS_CAPACITY_EXCEEDED;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    selection->dispatching = true;
    /* No early return is allowed in this loop: even a failed or cancelled
     * request must leave the model unlocked and the remaining requests reported. */
    for (index = 0U; index < selection->count; ++index) {
        UmiApplicationLaunchChoice *choice = &selection->choices[index];
        UmiApplicationLaunchDispatchResult *result;
        UmiStatus status;
        if (!choice->selected) continue;
        result = &out_report->results[out_report->result_count];
        status = copy_text(result->application_id, sizeof(result->application_id),
                           choice->application_id, false);
        if (status == UMI_STATUS_OK) {
            status = dispatch_eligibility(selection, choice);
        }
        /* The callback receives identity only; it owns actual launch/activation
         * and can reject a request without the model changing runtime state. */
        if (status == UMI_STATUS_OK) {
            status = callback(choice->application_id, context);
        }
        result->status = status;
        out_report->result_count += 1U;
        if (status == UMI_STATUS_OK) {
            choice->selected = false;
            selection->revision += 1U;
            out_report->accepted_count += 1U;
        } else {
            /* Failed requests stay checked, so Retry addresses only failures. */
            out_report->failed_count += 1U;
            if (out_report->first_failure == UMI_STATUS_OK) {
                out_report->first_failure = status;
            }
        }
    }
    selection->dispatching = false;
    /* An empty click is not a successful launch and must be distinguishable
     * from a completed group of accepted requests. */
    if (out_report->result_count == 0U) {
        out_report->first_failure = UMI_STATUS_INVALID_STATE;
    }
    out_report->revision = selection->revision;
    return out_report->first_failure;
}
