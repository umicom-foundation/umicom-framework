/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/installer_selection.c
 *
 * PURPOSE:
 *   Implement a bounded installer checkbox model with required-component and
 *   disk-space safeguards.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/delivery/installer_selection.h"

#include <limits.h>
#include <string.h>

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
static UmiInstallerApplicationOption *find_mutable(
    UmiInstallerSelection *selection,
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
        if (strcmp(selection->options[index].application_id,
                   application_id) == 0) {
            return &selection->options[index];
        }
    }
    return NULL;
}

/* Provide the find const operation used by this module and its client applications. */
static const UmiInstallerApplicationOption *find_const(
    const UmiInstallerSelection *selection,
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
        if (strcmp(selection->options[index].application_id,
                   application_id) == 0) {
            return &selection->options[index];
        }
    }
    return NULL;
}

/*
 * Initialise installer selection from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_installer_selection_init(UmiInstallerSelection *selection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(selection, 0, sizeof(*selection));
    selection->revision = 1U;
    return UMI_STATUS_OK;
}

/* Add installer selection only after its inputs and available capacity have been checked. */
UmiStatus umi_installer_selection_add(
    UmiInstallerSelection *selection,
    const UmiInstallerApplicationDefinition *definition)
{
    UmiInstallerApplicationOption *option;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || definition == NULL ||
        definition->structure_size <
            sizeof(UmiInstallerApplicationDefinition) ||
        definition->application_id == NULL ||
        definition->display_name == NULL ||
        definition->component_name == NULL ||
        definition->executable_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (find_mutable(selection, definition->application_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (selection->count >= UMI_INSTALLER_SELECTION_MAX_APPLICATIONS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    option = &selection->options[selection->count];
    (void)memset(option, 0, sizeof(*option));
    status = copy_text(option->application_id,
                       sizeof(option->application_id),
                       definition->application_id, false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(option->display_name,
                           sizeof(option->display_name),
                           definition->display_name, false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(option->description,
                           sizeof(option->description),
                           definition->description, true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(option->component_name,
                           sizeof(option->component_name),
                           definition->component_name, false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(option->executable_name,
                           sizeof(option->executable_name),
                           definition->executable_name, false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    option->estimated_size_bytes = definition->estimated_size_bytes;
    option->required = definition->required;
    option->available = definition->available;
    option->selected = definition->available &&
        (definition->required || definition->selected_by_default);
    selection->count += 1U;
    selection->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find installer selection set while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_installer_selection_set_selected(
    UmiInstallerSelection *selection,
    const char *application_id,
    bool selected)
{
    UmiInstallerApplicationOption *option = find_mutable(
        selection, application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (option == NULL) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (selected && !option->available) return UMI_STATUS_UNAVAILABLE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!selected && option->required) return UMI_STATUS_PERMISSION_DENIED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (option->selected != selected) {
        option->selected = selected;
        selection->revision += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the installer selection select all operation used by this module and its client
 * applications.
 */
UmiStatus umi_installer_selection_select_all(
    UmiInstallerSelection *selection)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->count; ++index) {
        selection->options[index].selected =
            selection->options[index].available;
    }
    selection->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the installer selection clear optional operation used by this module and its
 * client applications.
 */
UmiStatus umi_installer_selection_clear_optional(
    UmiInstallerSelection *selection)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->count; ++index) {
        UmiInstallerApplicationOption *option = &selection->options[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!option->required) option->selected = false;
    }
    selection->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find installer selection while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_installer_selection_at(
    const UmiInstallerSelection *selection,
    size_t index,
    UmiInstallerApplicationOption *out_option)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || out_option == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= selection->count) return UMI_STATUS_NOT_FOUND;
    *out_option = selection->options[index];
    return UMI_STATUS_OK;
}

/*
 * Find installer selection while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_installer_selection_find(
    const UmiInstallerSelection *selection,
    const char *application_id,
    UmiInstallerApplicationOption *out_option)
{
    const UmiInstallerApplicationOption *option;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_option == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    option = find_const(selection, application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (option == NULL) return UMI_STATUS_NOT_FOUND;
    *out_option = *option;
    return UMI_STATUS_OK;
}

/*
 * Provide the installer selection snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_installer_selection_snapshot(
    const UmiInstallerSelection *selection,
    UmiInstallerSelectionSnapshot *out_snapshot)
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
    out_snapshot->application_count = selection->count;
    out_snapshot->revision = selection->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->count; ++index) {
        const UmiInstallerApplicationOption *option =
            &selection->options[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (option->available) out_snapshot->available_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (option->required) out_snapshot->required_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (option->selected) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (UINT64_MAX - out_snapshot->selected_size_bytes <
                option->estimated_size_bytes) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_snapshot->selected_count += 1U;
            out_snapshot->selected_size_bytes +=
                option->estimated_size_bytes;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Check that installer selection satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_installer_selection_validate(
    const UmiInstallerSelection *selection)
{
    size_t index;
    UmiInstallerSelectionSnapshot snapshot;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || selection->count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < selection->count; ++index) {
        const UmiInstallerApplicationOption *option =
            &selection->options[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (option->application_id[0] == '\0' ||
            option->display_name[0] == '\0' ||
            option->component_name[0] == '\0' ||
            option->executable_name[0] == '\0' ||
            (option->required && (!option->available || !option->selected))) {
            return UMI_STATUS_INVALID_STATE;
        }
    }
    return umi_installer_selection_snapshot(selection, &snapshot);
}
