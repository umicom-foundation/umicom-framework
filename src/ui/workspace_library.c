/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workspace_library.c
 * PURPOSE: Atomic named-layout operations over the existing customisation
 * object, with copied observations and no persistent parallel catalogue.
 * AUTHOR: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/workspace_library.h"
#include <stdlib.h>
#include <string.h>

/* Check actual fixed-array boundaries before a lookup or diagnostic uses text. */
static bool fixed_text(const char *text, size_t capacity, bool required)
{
    return memchr(text, '\0', capacity) != NULL && (!required || text[0] != '\0');
}

/* Borrowed request strings may point into the live model; copy before commit. */
static UmiStatus copy_request_text(char *output, size_t capacity,
                                   const char *text, bool required)
{
    size_t length = 0U;
    if (text == NULL) {
        if (required) return UMI_STATUS_INVALID_ARGUMENT;
        output[0] = '\0';
        return UMI_STATUS_OK;
    }
    while (length < capacity && text[length] != '\0') ++length;
    if (length == capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (required && length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memcpy(output, text, length + 1U);
    return UMI_STATUS_OK;
}

/* Layout identifiers are semantic tokens; display names remain separate text. */
static bool identifier_valid(const char *text)
{
    const unsigned char *cursor = (const unsigned char *)text;
    if (!((*cursor >= 'A' && *cursor <= 'Z') || (*cursor >= 'a' && *cursor <= 'z') ||
          (*cursor >= '0' && *cursor <= '9'))) return false;
    for (; *cursor != 0U; ++cursor)
        if (!((*cursor >= 'A' && *cursor <= 'Z') || (*cursor >= 'a' && *cursor <= 'z') ||
              (*cursor >= '0' && *cursor <= '9') || *cursor == '.' ||
              *cursor == '_' || *cursor == '-')) return false;
    return true;
}

/* Validate Unicode scalars locally: the existing document UTF-8 validator is
 * in a higher-level library which must not become a reverse UI dependency.
 * Names exclude ASCII control bytes and overlong/surrogate/out-of-range text,
 * matching GTK's requirement that copied label strings be valid UTF-8. */
static bool name_valid(const char *text)
{
    const unsigned char *cursor = (const unsigned char *)text;
    bool meaningful = false;
    while (*cursor != 0U) {
        uint32_t scalar;
        uint32_t minimum;
        size_t remaining;
        const unsigned char lead = *cursor++;
        if (lead < 0x80U) {
            if (lead < 32U || lead == 127U) return false;
            if (lead != ' ') meaningful = true;
            continue;
        }
        if (lead >= 0xC2U && lead <= 0xDFU) {
            scalar = lead & 0x1FU; minimum = 0x80U; remaining = 1U;
        } else if (lead >= 0xE0U && lead <= 0xEFU) {
            scalar = lead & 0x0FU; minimum = 0x800U; remaining = 2U;
        } else if (lead >= 0xF0U && lead <= 0xF4U) {
            scalar = lead & 0x07U; minimum = 0x10000U; remaining = 3U;
        } else return false;
        while (remaining != 0U) {
            const unsigned char continuation = *cursor;
            if (continuation < 0x80U || continuation > 0xBFU) return false;
            scalar = (scalar << 6U) | (uint32_t)(continuation & 0x3FU);
            ++cursor;
            --remaining;
        }
        if (scalar < minimum || scalar > 0x10FFFFU ||
            (scalar >= 0xD800U && scalar <= 0xDFFFU)) return false;
        meaningful = true;
    }
    return meaningful;
}

/* Avoid publishing a copied snapshot over its still-live owning model. */
static bool overlaps_model(const UmiUiWorkspaceCustomisation *model,
                            const UmiUiWorkspaceLibrarySnapshot *snapshot)
{
    const uintptr_t model_address = (uintptr_t)(const void *)model;
    const uintptr_t snapshot_address = (uintptr_t)(const void *)snapshot;
    if (snapshot == NULL) return false;
    return model_address <= snapshot_address
        ? snapshot_address - model_address < sizeof(*model)
        : model_address - snapshot_address < sizeof(*snapshot);
}

/* Copy and validate a product boundary without retaining caller pointers. */
static UmiStatus copy_prefix(const UmiUiWorkspaceLibraryPolicy *policy,
                              char *prefix, size_t capacity)
{
    UmiStatus status = copy_request_text(prefix, capacity,
        policy != NULL ? policy->layout_prefix : NULL, false);
    size_t length;
    if (status != UMI_STATUS_OK) return status;
    length = strlen(prefix);
    if (length != 0U && (!identifier_valid(prefix) || prefix[length - 1U] != '.'))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* An owning prefix includes its final separator and requires a nonempty suffix. */
static bool prefix_matches(const char *prefix, const char *id)
{
    const size_t length = strlen(prefix);
    return length == 0U || (strncmp(prefix, id, length) == 0 && id[length] != '\0');
}

/* Validate all text consumed by the existing geometry diagnostic and by future
 * native renderers before calling the established layout validator. */
static UmiStatus validate_layout(const UmiUiWorkspaceLayout *layout)
{
    size_t index;
    char reason[192U];
    if (!fixed_text(layout->layout_id, sizeof(layout->layout_id), true) ||
        !identifier_valid(layout->layout_id) ||
        !fixed_text(layout->name, sizeof(layout->name), true) ||
        !name_valid(layout->name) ||
        layout->window_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS)
        return UMI_STATUS_INVALID_STATE;
    for (index = 0U; index < layout->window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &layout->windows[index];
        size_t previous;
        if (!fixed_text(window->window_id, sizeof(window->window_id), true) ||
            !fixed_text(window->title, sizeof(window->title), false) ||
            !fixed_text(window->tool_id, sizeof(window->tool_id), false) ||
            !fixed_text(window->group_id, sizeof(window->group_id), false) ||
            !fixed_text(window->placement_id, sizeof(window->placement_id), false) ||
            !fixed_text(window->stack_id, sizeof(window->stack_id), false) ||
            !fixed_text(window->context_group_id, sizeof(window->context_group_id), false))
            return UMI_STATUS_INVALID_STATE;
        for (previous = 0U; previous < index; ++previous)
            if (strcmp(window->window_id, layout->windows[previous].window_id) == 0)
                return UMI_STATUS_INVALID_STATE;
    }
    return umi_ui_workspace_layout_validate(layout, reason, sizeof(reason)) == UMI_STATUS_OK
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

/* Context membership remains owned by customisation and is never rewritten by
 * a library operation. Bound its existing records before copying the owner. */
static UmiStatus validate_groups(const UmiUiWindowGroupStore *groups)
{
    size_t index;
    if (groups->count > UMI_UI_WINDOW_GROUP_MAX) return UMI_STATUS_INVALID_STATE;
    for (index = 0U; index < groups->count; ++index) {
        const UmiUiWindowGroup *group = &groups->items[index];
        size_t member;
        if (!fixed_text(group->group_id, sizeof(group->group_id), true) ||
            !fixed_text(group->colour_token, sizeof(group->colour_token), false) ||
            group->member_count > UMI_UI_WINDOW_GROUP_MAX_MEMBERS)
            return UMI_STATUS_INVALID_STATE;
        for (member = 0U; member < group->member_count; ++member)
            if (!fixed_text(group->members[member].window_id,
                            sizeof(group->members[member].window_id), true))
                return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* Validate bounded owned records without inventing another semantic catalogue.
 * An optional prefix owns the whole stored list, so active removal cannot
 * select an unrelated product's layout as its deterministic fallback. */
static UmiStatus validate_model(const UmiUiWorkspaceCustomisation *model,
                                const char *prefix)
{
    size_t index;
    bool active_found = false;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (model->layout_count > UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS ||
        model->windows.count > UMI_UI_WINDOW_CATALOGUE_MAX ||
        model->windows.recent_count > UMI_UI_WINDOW_RECENT_MAX ||
        model->library.count > UMI_UI_LAYOUT_LIBRARY_MAX ||
        !fixed_text(model->active_layout_id, sizeof(model->active_layout_id), model->layout_count != 0U))
        return UMI_STATUS_INVALID_STATE;
    for (index = 0U; index < model->layout_count; ++index) {
        const UmiUiWorkspaceLayout *layout = &model->layouts[index];
        size_t previous;
        if (validate_layout(layout) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
        if (!prefix_matches(prefix, layout->layout_id)) return UMI_STATUS_PERMISSION_DENIED;
        for (previous = 0U; previous < index; ++previous)
            if (strcmp(layout->layout_id, model->layouts[previous].layout_id) == 0)
                return UMI_STATUS_INVALID_STATE;
        if (strcmp(layout->layout_id, model->active_layout_id) == 0) active_found = true;
    }
    if ((model->layout_count != 0U && !active_found) ||
        (model->layout_count == 0U && model->active_layout_id[0] != '\0'))
        return UMI_STATUS_INVALID_STATE;
    if (validate_groups(&model->groups) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
    for (index = 0U; index < model->windows.count; ++index) {
        const UmiUiWindowDescriptor *window = &model->windows.items[index];
        if (!fixed_text(window->tool_id, sizeof(window->tool_id), true) ||
            !fixed_text(window->title, sizeof(window->title), true) ||
            !fixed_text(window->description, sizeof(window->description), false) ||
            !fixed_text(window->icon_name, sizeof(window->icon_name), false))
            return UMI_STATUS_INVALID_STATE;
    }
    for (index = 0U; index < model->windows.recent_count; ++index)
        if (!fixed_text(model->windows.recent[index].tool_id,
                        sizeof(model->windows.recent[index].tool_id), true))
            return UMI_STATUS_INVALID_STATE;
    for (index = 0U; index < model->library.count; ++index) {
        const UmiUiLayoutLibraryItem *item = &model->library.items[index];
        if (!fixed_text(item->preset_id, sizeof(item->preset_id), true) ||
            !fixed_text(item->category, sizeof(item->category), false) ||
            !fixed_text(item->description, sizeof(item->description), false) ||
            validate_layout(&item->layout) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_STATE;
    }
    if (model->edit_active && (validate_layout(&model->edit_baseline) != UMI_STATUS_OK ||
        validate_groups(&model->edit_groups_baseline) != UMI_STATUS_OK))
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/* Copy a fully validated list; no failure or borrowed text remains afterward. */
static void copy_snapshot(const UmiUiWorkspaceCustomisation *model,
                           UmiUiWorkspaceLibrarySnapshot *snapshot)
{
    size_t index;
    if (snapshot == NULL) return;
    (void)memset(snapshot, 0, sizeof(*snapshot));
    snapshot->layout_count = model->layout_count;
    snapshot->customisation_revision = model->revision;
    snapshot->editing = model->edit_active;
    for (index = 0U; index < model->layout_count; ++index) {
        const UmiUiWorkspaceLayout *layout = &model->layouts[index];
        UmiUiWorkspaceLibraryRow *row = &snapshot->rows[index];
        (void)memcpy(row->layout_id, layout->layout_id, strlen(layout->layout_id) + 1U);
        (void)memcpy(row->name, layout->name, strlen(layout->name) + 1U);
        row->window_count = layout->window_count;
        row->active = strcmp(layout->layout_id, model->active_layout_id) == 0;
        row->locked = layout->locked;
    }
}

/* Observe named layouts without changing model selection, lock or edit state. */
UmiStatus umi_ui_workspace_library_snapshot(
    const UmiUiWorkspaceCustomisation *customisation,
    const UmiUiWorkspaceLibraryPolicy *policy,
    UmiUiWorkspaceLibrarySnapshot *out_snapshot)
{
    char prefix[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    UmiStatus status;
    if (customisation == NULL || out_snapshot == NULL || overlaps_model(customisation, out_snapshot))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = copy_prefix(policy, prefix, sizeof(prefix));
    if (status == UMI_STATUS_OK) status = validate_model(customisation, prefix);
    if (status == UMI_STATUS_OK) copy_snapshot(customisation, out_snapshot);
    return status;
}

/* Look up only after all fixed-width identifiers have passed validation. */
static size_t find_layout(const UmiUiWorkspaceCustomisation *model, const char *id)
{
    size_t index;
    for (index = 0U; index < model->layout_count; ++index)
        if (strcmp(model->layouts[index].layout_id, id) == 0) return index;
    return model->layout_count;
}

/* Publish one guarded named-layout operation and preserve every other owner. */
UmiStatus umi_ui_workspace_library_apply(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiUiWorkspaceLibraryPolicy *policy,
    const UmiUiWorkspaceLibraryRequest *request,
    UmiUiWorkspaceLibrarySnapshot *out_snapshot)
{
    UmiUiWorkspaceCustomisation *candidate;
    UmiUiWorkspaceLibraryRequest operation;
    char prefix[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char target[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char new_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char name[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY];
    size_t target_index;
    UmiStatus status;
    bool no_change = false;
    if (customisation == NULL || request == NULL || overlaps_model(customisation, out_snapshot))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (customisation->edit_active) return UMI_STATUS_BUSY;
    operation = *request;
    if (operation.action < UMI_UI_WORKSPACE_LIBRARY_DUPLICATE ||
        operation.action > UMI_UI_WORKSPACE_LIBRARY_ACTIVATE)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = copy_prefix(policy, prefix, sizeof(prefix));
    if (status == UMI_STATUS_OK) status = validate_model(customisation, prefix);
    if (status != UMI_STATUS_OK) return status;
    if (operation.expected_customisation_revision != customisation->revision)
        return UMI_STATUS_INVALID_STATE;
    status = copy_request_text(target, sizeof(target), operation.target_layout_id, true);
    if (status != UMI_STATUS_OK) return status;
    if (!identifier_valid(target)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!prefix_matches(prefix, target)) return UMI_STATUS_PERMISSION_DENIED;
    target_index = find_layout(customisation, target);
    if (target_index == customisation->layout_count) return UMI_STATUS_NOT_FOUND;
    new_id[0] = '\0';
    name[0] = '\0';
    if (operation.action == UMI_UI_WORKSPACE_LIBRARY_DUPLICATE) {
        status = copy_request_text(new_id, sizeof(new_id), operation.new_layout_id, true);
        if (status != UMI_STATUS_OK) return status;
        if (!identifier_valid(new_id)) return UMI_STATUS_INVALID_ARGUMENT;
        if (!prefix_matches(prefix, new_id)) return UMI_STATUS_PERMISSION_DENIED;
        if (find_layout(customisation, new_id) != customisation->layout_count)
            return UMI_STATUS_ALREADY_EXISTS;
        if (customisation->layout_count == UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (operation.action == UMI_UI_WORKSPACE_LIBRARY_DUPLICATE ||
        operation.action == UMI_UI_WORKSPACE_LIBRARY_RENAME) {
        status = copy_request_text(name, sizeof(name), operation.name, true);
        if (status != UMI_STATUS_OK) return status;
        if (!name_valid(name)) return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (operation.action == UMI_UI_WORKSPACE_LIBRARY_REMOVE) {
        if (!operation.confirmed) return UMI_STATUS_PERMISSION_DENIED;
        if (customisation->layout_count <= 1U) return UMI_STATUS_INVALID_STATE;
    }
    if (operation.action == UMI_UI_WORKSPACE_LIBRARY_RENAME)
        no_change = strcmp(customisation->layouts[target_index].name, name) == 0;
    if (operation.action == UMI_UI_WORKSPACE_LIBRARY_ACTIVATE)
        no_change = strcmp(customisation->active_layout_id, target) == 0;
    if (no_change) {
        copy_snapshot(customisation, out_snapshot);
        return UMI_STATUS_OK;
    }
    if (customisation->revision == UINT64_MAX ||
        (operation.action == UMI_UI_WORKSPACE_LIBRARY_RENAME &&
         customisation->layouts[target_index].revision == UINT64_MAX))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate = (UmiUiWorkspaceCustomisation *)malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *customisation;
    switch (operation.action) {
    case UMI_UI_WORKSPACE_LIBRARY_DUPLICATE:
        /* Clone directly into unused candidate storage, avoiding the older
         * convenience helper's large automatic layout object. */
        status = umi_ui_workspace_layout_clone(&candidate->layouts[target_index],
            new_id, name, &candidate->layouts[candidate->layout_count]);
        if (status == UMI_STATUS_OK) {
            ++candidate->layout_count;
            status = umi_ui_workspace_customisation_activate(candidate, new_id);
        }
        break;
    case UMI_UI_WORKSPACE_LIBRARY_RENAME: {
        const bool locked = candidate->layouts[target_index].locked;
        /* Library naming is metadata, not a geometry edit. The private
         * temporary unlock never escapes and original locking is restored. */
        candidate->layouts[target_index].locked = false;
        status = umi_ui_workspace_layout_rename(&candidate->layouts[target_index], name);
        candidate->layouts[target_index].locked = locked;
        if (status == UMI_STATUS_OK) ++candidate->revision;
        break;
    }
    case UMI_UI_WORKSPACE_LIBRARY_REMOVE:
        status = umi_ui_workspace_customisation_remove_layout(candidate, target);
        if (status == UMI_STATUS_OK)
            (void)memset(&candidate->layouts[candidate->layout_count], 0, sizeof(candidate->layouts[0]));
        break;
    case UMI_UI_WORKSPACE_LIBRARY_ACTIVATE:
        status = umi_ui_workspace_customisation_activate(candidate, target);
        break;
    default:
        status = UMI_STATUS_INVALID_ARGUMENT;
        break;
    }
    if (status == UMI_STATUS_OK) status = validate_model(candidate, prefix);
    if (status == UMI_STATUS_OK) {
        *customisation = *candidate;
        copy_snapshot(customisation, out_snapshot);
    }
    free(candidate);
    return status;
}
