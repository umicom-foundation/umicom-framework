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

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source,
                           bool allow_empty)
{
    size_t length;
    if (destination == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (source == NULL || source[0] == '\0') {
        if (!allow_empty) return UMI_STATUS_INVALID_ARGUMENT;
        destination[0] = '\0';
        return UMI_STATUS_OK;
    }
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static UmiInstallerApplicationOption *find_mutable(
    UmiInstallerSelection *selection,
    const char *application_id)
{
    size_t index;
    if (selection == NULL || application_id == NULL ||
        application_id[0] == '\0') {
        return NULL;
    }
    for (index = 0U; index < selection->count; ++index) {
        if (strcmp(selection->options[index].application_id,
                   application_id) == 0) {
            return &selection->options[index];
        }
    }
    return NULL;
}

static const UmiInstallerApplicationOption *find_const(
    const UmiInstallerSelection *selection,
    const char *application_id)
{
    size_t index;
    if (selection == NULL || application_id == NULL ||
        application_id[0] == '\0') {
        return NULL;
    }
    for (index = 0U; index < selection->count; ++index) {
        if (strcmp(selection->options[index].application_id,
                   application_id) == 0) {
            return &selection->options[index];
        }
    }
    return NULL;
}

UmiStatus umi_installer_selection_init(UmiInstallerSelection *selection)
{
    if (selection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(selection, 0, sizeof(*selection));
    selection->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_installer_selection_add(
    UmiInstallerSelection *selection,
    const UmiInstallerApplicationDefinition *definition)
{
    UmiInstallerApplicationOption *option;
    UmiStatus status;
    if (selection == NULL || definition == NULL ||
        definition->structure_size <
            sizeof(UmiInstallerApplicationDefinition) ||
        definition->application_id == NULL ||
        definition->display_name == NULL ||
        definition->component_name == NULL ||
        definition->executable_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (find_mutable(selection, definition->application_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (selection->count >= UMI_INSTALLER_SELECTION_MAX_APPLICATIONS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    option = &selection->options[selection->count];
    (void)memset(option, 0, sizeof(*option));
    status = copy_text(option->application_id,
                       sizeof(option->application_id),
                       definition->application_id, false);
    if (status == UMI_STATUS_OK) {
        status = copy_text(option->display_name,
                           sizeof(option->display_name),
                           definition->display_name, false);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(option->description,
                           sizeof(option->description),
                           definition->description, true);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(option->component_name,
                           sizeof(option->component_name),
                           definition->component_name, false);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(option->executable_name,
                           sizeof(option->executable_name),
                           definition->executable_name, false);
    }
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

UmiStatus umi_installer_selection_set_selected(
    UmiInstallerSelection *selection,
    const char *application_id,
    bool selected)
{
    UmiInstallerApplicationOption *option = find_mutable(
        selection, application_id);
    if (option == NULL) return UMI_STATUS_NOT_FOUND;
    if (selected && !option->available) return UMI_STATUS_UNAVAILABLE;
    if (!selected && option->required) return UMI_STATUS_PERMISSION_DENIED;
    if (option->selected != selected) {
        option->selected = selected;
        selection->revision += 1U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_installer_selection_select_all(
    UmiInstallerSelection *selection)
{
    size_t index;
    if (selection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < selection->count; ++index) {
        selection->options[index].selected =
            selection->options[index].available;
    }
    selection->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_installer_selection_clear_optional(
    UmiInstallerSelection *selection)
{
    size_t index;
    if (selection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < selection->count; ++index) {
        UmiInstallerApplicationOption *option = &selection->options[index];
        if (!option->required) option->selected = false;
    }
    selection->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_installer_selection_at(
    const UmiInstallerSelection *selection,
    size_t index,
    UmiInstallerApplicationOption *out_option)
{
    if (selection == NULL || out_option == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= selection->count) return UMI_STATUS_NOT_FOUND;
    *out_option = selection->options[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_installer_selection_find(
    const UmiInstallerSelection *selection,
    const char *application_id,
    UmiInstallerApplicationOption *out_option)
{
    const UmiInstallerApplicationOption *option;
    if (out_option == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    option = find_const(selection, application_id);
    if (option == NULL) return UMI_STATUS_NOT_FOUND;
    *out_option = *option;
    return UMI_STATUS_OK;
}

UmiStatus umi_installer_selection_snapshot(
    const UmiInstallerSelection *selection,
    UmiInstallerSelectionSnapshot *out_snapshot)
{
    size_t index;
    if (selection == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->application_count = selection->count;
    out_snapshot->revision = selection->revision;
    for (index = 0U; index < selection->count; ++index) {
        const UmiInstallerApplicationOption *option =
            &selection->options[index];
        if (option->available) out_snapshot->available_count += 1U;
        if (option->required) out_snapshot->required_count += 1U;
        if (option->selected) {
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

UmiStatus umi_installer_selection_validate(
    const UmiInstallerSelection *selection)
{
    size_t index;
    UmiInstallerSelectionSnapshot snapshot;
    if (selection == NULL || selection->count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < selection->count; ++index) {
        const UmiInstallerApplicationOption *option =
            &selection->options[index];
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
