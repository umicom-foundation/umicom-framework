/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/component_drag_drop.c
 *
 * PURPOSE:
 *   Implement toolkit-neutral component drag-and-drop under the federated
 *   desktop authority. No application directly moves or copies another
 *   application's window.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/component_drag_drop.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/application/component_catalogue.h"

typedef struct UmiDesktopComponentDragEntry {
    UmiDesktopComponentDragSnapshot snapshot;
} UmiDesktopComponentDragEntry;

struct UmiDesktopComponentDragDrop {
    UmiDesktopRuntime *desktop;
    UmiDesktopContentRuntime *content;
    UmiUiDragDropRegistry *registry;
    UmiDesktopComponentDragEntry entries[UMI_DESKTOP_COMPONENT_DRAG_MAX];
    size_t count;
    uint64_t revision;
};

/* Provide the find entry operation used by this module and its client applications. */
static size_t find_entry(
    const UmiDesktopComponentDragDrop *drag_drop,
    const char *session_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (drag_drop == NULL || session_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < drag_drop->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(drag_drop->entries[index].snapshot.session_id,
                   session_id) == 0) return index;
    }
    return SIZE_MAX;
}

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(
    char *destination,
    size_t capacity,
    const char *source,
    int required)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL ||
        (required && source[0] == '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the sync registry operation used by this module and its client applications. */
static UmiStatus sync_registry(
    UmiDesktopComponentDragDrop *drag_drop,
    const UmiDesktopComponentDragSnapshot *snapshot)
{
    UmiUiDragDropSnapshot item;
    const char *target_id;
    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = 1U;
    (void)snprintf(item.id, sizeof(item.id), "%s", snapshot->session_id);
    (void)snprintf(item.source_id, sizeof(item.source_id), "%s",
                   snapshot->source_window_id);
    target_id = snapshot->operation == UMI_DESKTOP_COMPONENT_DRAG_LINK
        ? snapshot->target.target_window_id
        : snapshot->target.monitor_id;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (target_id[0] != '\0')
        (void)snprintf(item.target_id, sizeof(item.target_id), "%s", target_id);
    (void)snprintf(item.mime_type, sizeof(item.mime_type), "%s",
                   "application/x-umicom-component-window");
    (void)snprintf(item.payload, sizeof(item.payload), "%s",
                   snapshot->source_component_id);
    item.allowed = snapshot->allowed;
    item.copy = snapshot->operation == UMI_DESKTOP_COMPONENT_DRAG_COPY;
    item.move = snapshot->operation == UMI_DESKTOP_COMPONENT_DRAG_MOVE;
    item.link = snapshot->operation == UMI_DESKTOP_COMPONENT_DRAG_LINK;
    return umi_ui_drag_drop_registry_upsert(drag_drop->registry, &item);
}

/* Provide the mark entry operation used by this module and its client applications. */
static UmiStatus mark_entry(
    UmiDesktopComponentDragDrop *drag_drop,
    UmiDesktopComponentDragEntry *entry,
    UmiDesktopComponentDragState state,
    int allowed,
    UmiStatus status)
{
    entry->snapshot.state = state;
    entry->snapshot.allowed = allowed;
    entry->snapshot.last_status = status;
    entry->snapshot.revision += 1U;
    drag_drop->revision += 1U;
    return sync_registry(drag_drop, &entry->snapshot);
}

/* Provide the validate target operation used by this module and its client applications. */
static UmiStatus validate_target(
    UmiDesktopComponentDragDrop *drag_drop,
    const UmiDesktopComponentDragSnapshot *snapshot,
    const UmiDesktopComponentDropTarget *target)
{
    const UmiApplicationComponentDefinition *component;
    const UmiDesktopWindow *target_window;
    /* Configure the optional target only when its feature has created it. */
    if (target->structure_size < sizeof(*target))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (snapshot->operation == UMI_DESKTOP_COMPONENT_DRAG_LINK) {
        /* Configure the optional target only when its feature has created it. */
        if (target->target_window_id[0] == '\0' ||
            strcmp(target->target_window_id,
                   snapshot->source_window_id) == 0)
            return UMI_STATUS_INVALID_ARGUMENT;
        target_window = umi_desktop_window_manager_find(
            umi_desktop_runtime_windows(drag_drop->desktop),
            target->target_window_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (target_window == NULL) return UMI_STATUS_NOT_FOUND;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (target_window->context_group_id[0] == '\0')
            return UMI_STATUS_INVALID_STATE;
        /* Configure the optional target only when its feature has created it. */
        if (target->context_role < UMI_UI_WINDOW_GROUP_SOURCE ||
            target->context_role > UMI_UI_WINDOW_GROUP_BIDIRECTIONAL)
            return UMI_STATUS_INVALID_ARGUMENT;
        return UMI_STATUS_OK;
    }
    /* Configure the optional target only when its feature has created it. */
    if (target->monitor_id[0] == '\0' || target->bounds.width <= 0 ||
        target->bounds.height <= 0 ||
        target->placement < UMI_DESKTOP_DOCK_CANVAS ||
        target->placement > UMI_DESKTOP_DOCK_FLOATING)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_desktop_monitor_topology_find(
            umi_desktop_runtime_monitors(drag_drop->desktop),
            target->monitor_id) == NULL)
        return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (snapshot->operation == UMI_DESKTOP_COMPONENT_DRAG_COPY) {
        component = umi_application_component_catalogue_find(
            snapshot->source_component_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (component == NULL) return UMI_STATUS_NOT_FOUND;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (!component->multi_instance) return UMI_STATUS_PERMISSION_DENIED;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_ui_id_is_valid(target->new_window_id) ||
            !umi_ui_id_is_valid(target->new_view_id) ||
            target->title[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_desktop_window_manager_find(
                umi_desktop_runtime_windows(drag_drop->desktop),
                target->new_window_id) != NULL)
            return UMI_STATUS_ALREADY_EXISTS;
        /* Configure the optional target only when its feature has created it. */
        if (target->context_role < UMI_UI_WINDOW_GROUP_SOURCE ||
            target->context_role > UMI_UI_WINDOW_GROUP_BIDIRECTIONAL)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise desktop component drag drop from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_desktop_component_drag_drop_create(
    UmiDesktopRuntime *desktop,
    UmiDesktopContentRuntime *content,
    UmiDesktopComponentDragDrop **out_drag_drop)
{
    UmiDesktopComponentDragDrop *drag_drop;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desktop == NULL || content == NULL || out_drag_drop == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_drag_drop = NULL;
    drag_drop = (UmiDesktopComponentDragDrop *)calloc(1U, sizeof(*drag_drop));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (drag_drop == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    drag_drop->desktop = desktop;
    drag_drop->content = content;
    status = umi_ui_drag_drop_registry_create(&drag_drop->registry);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(drag_drop);
        return status;
    }
    *out_drag_drop = drag_drop;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by desktop component drag drop so the same storage can be
 * reused safely.
 */
void umi_desktop_component_drag_drop_destroy(
    UmiDesktopComponentDragDrop *drag_drop)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (drag_drop == NULL) return;
    umi_ui_drag_drop_registry_destroy(drag_drop->registry);
    free(drag_drop);
}

/*
 * Provide the desktop component drag drop registry operation used by this module and its
 * client applications.
 */
UmiUiDragDropRegistry *umi_desktop_component_drag_drop_registry(
    UmiDesktopComponentDragDrop *drag_drop)
{
    return drag_drop != NULL ? drag_drop->registry : NULL;
}

/*
 * Provide the desktop component drag begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_component_drag_begin(
    UmiDesktopComponentDragDrop *drag_drop,
    const char *session_id,
    const char *source_window_id,
    UmiDesktopComponentDragOperation operation)
{
    UmiDesktopContentSnapshot content;
    UmiDesktopComponentDragEntry *entry;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (drag_drop == NULL || !umi_ui_id_is_valid(session_id) ||
        !umi_ui_id_is_valid(source_window_id) ||
        operation < UMI_DESKTOP_COMPONENT_DRAG_MOVE ||
        operation > UMI_DESKTOP_COMPONENT_DRAG_LINK)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_entry(drag_drop, session_id) != SIZE_MAX)
        return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (drag_drop->count >= UMI_DESKTOP_COMPONENT_DRAG_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_desktop_content_runtime_snapshot(
        drag_drop->content, source_window_id, &content);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    entry = &drag_drop->entries[drag_drop->count];
    (void)memset(entry, 0, sizeof(*entry));
    entry->snapshot.structure_size = (uint32_t)sizeof(entry->snapshot);
    entry->snapshot.operation = operation;
    entry->snapshot.state = UMI_DESKTOP_COMPONENT_DRAG_STARTED;
    entry->snapshot.last_status = UMI_STATUS_OK;
    entry->snapshot.revision = 1U;
    status = copy_text(entry->snapshot.session_id,
                       sizeof(entry->snapshot.session_id), session_id, 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(entry->snapshot.source_window_id,
                           sizeof(entry->snapshot.source_window_id),
                           source_window_id, 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(entry->snapshot.source_component_id,
                           sizeof(entry->snapshot.source_component_id),
                           content.window.component_id, 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(entry->snapshot.source_application_id,
                           sizeof(entry->snapshot.source_application_id),
                           content.window.owner_application_id, 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    drag_drop->count += 1U;
    drag_drop->revision += 1U;
    status = sync_registry(drag_drop, &entry->snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        drag_drop->count -= 1U;
        (void)memset(entry, 0, sizeof(*entry));
    }
    return status;
}

/*
 * Provide the desktop component drag target operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_component_drag_target(
    UmiDesktopComponentDragDrop *drag_drop,
    const char *session_id,
    const UmiDesktopComponentDropTarget *target)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (drag_drop == NULL || session_id == NULL || target == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_entry(drag_drop, session_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (drag_drop->entries[index].snapshot.state !=
        UMI_DESKTOP_COMPONENT_DRAG_STARTED)
        return UMI_STATUS_INVALID_STATE;
    drag_drop->entries[index].snapshot.target = *target;
    status = validate_target(
        drag_drop, &drag_drop->entries[index].snapshot, target);
    (void)mark_entry(
        drag_drop, &drag_drop->entries[index],
        status == UMI_STATUS_OK ? UMI_DESKTOP_COMPONENT_DRAG_TARGETED
                                : UMI_DESKTOP_COMPONENT_DRAG_FAILED,
        status == UMI_STATUS_OK, status);
    return status;
}

/* Provide the commit move operation used by this module and its client applications. */
static UmiStatus commit_move(
    UmiDesktopComponentDragDrop *drag_drop,
    const UmiDesktopComponentDragSnapshot *snapshot)
{
    return umi_desktop_runtime_place_window(
        drag_drop->desktop, snapshot->source_window_id,
        snapshot->target.monitor_id, snapshot->target.bounds,
        snapshot->target.placement);
}

/* Copy commit into module-owned storage so callers keep ownership of their input values. */
static UmiStatus commit_copy(
    UmiDesktopComponentDragDrop *drag_drop,
    const UmiDesktopComponentDragSnapshot *snapshot)
{
    UmiDesktopContentSnapshot source;
    UmiDesktopContentMountRequest mount;
    UmiStatus status = umi_desktop_content_runtime_snapshot(
        drag_drop->content, snapshot->source_window_id, &source);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&mount, 0, sizeof(mount));
    mount.structure_size = (uint32_t)sizeof(mount);
    mount.window = source.window;
    (void)snprintf(mount.window.window_id, sizeof(mount.window.window_id),
                   "%s", snapshot->target.new_window_id);
    (void)snprintf(mount.window.title, sizeof(mount.window.title), "%s",
                   snapshot->target.title);
    (void)snprintf(mount.window.monitor_id, sizeof(mount.window.monitor_id),
                   "%s", snapshot->target.monitor_id);
    mount.window.bounds = snapshot->target.bounds;
    mount.window.dock_placement = snapshot->target.placement;
    mount.window.visible = true;
    mount.window.closable = true;
    mount.window.maximised = false;
    (void)snprintf(mount.view_type, sizeof(mount.view_type), "%s",
                   source.host.descriptor.view_type);
    (void)snprintf(mount.view_id, sizeof(mount.view_id), "%s",
                   snapshot->target.new_view_id);
    mount.context_role = snapshot->target.context_role;
    return umi_desktop_content_runtime_mount(drag_drop->content, &mount);
}

/* Provide the commit link operation used by this module and its client applications. */
static UmiStatus commit_link(
    UmiDesktopComponentDragDrop *drag_drop,
    const UmiDesktopComponentDragSnapshot *snapshot)
{
    const UmiDesktopWindow *target = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(drag_drop->desktop),
        snapshot->target.target_window_id);
    /* Configure the optional target only when its feature has created it. */
    if (target == NULL) return UMI_STATUS_NOT_FOUND;
    return umi_desktop_content_runtime_link_context(
        drag_drop->content, snapshot->source_window_id,
        target->context_group_id, snapshot->target.context_role);
}

/*
 * Provide the desktop component drag commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_component_drag_commit(
    UmiDesktopComponentDragDrop *drag_drop,
    const char *session_id)
{
    UmiDesktopComponentDragEntry *entry;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (drag_drop == NULL || session_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_entry(drag_drop, session_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    entry = &drag_drop->entries[index];
    /* Apply this branch only when its contract condition is satisfied. */
    if (entry->snapshot.state != UMI_DESKTOP_COMPONENT_DRAG_TARGETED ||
        !entry->snapshot.allowed) return UMI_STATUS_INVALID_STATE;
    /* Select the behaviour associated with the requested command or state value. */
    switch (entry->snapshot.operation) {
        case UMI_DESKTOP_COMPONENT_DRAG_MOVE:
            status = commit_move(drag_drop, &entry->snapshot);
            break;
        case UMI_DESKTOP_COMPONENT_DRAG_COPY:
            status = commit_copy(drag_drop, &entry->snapshot);
            break;
        case UMI_DESKTOP_COMPONENT_DRAG_LINK:
            status = commit_link(drag_drop, &entry->snapshot);
            break;
        default:
            status = UMI_STATUS_INVALID_STATE;
            break;
    }
    (void)mark_entry(
        drag_drop, entry,
        status == UMI_STATUS_OK ? UMI_DESKTOP_COMPONENT_DRAG_COMMITTED
                                : UMI_DESKTOP_COMPONENT_DRAG_FAILED,
        0, status);
    return status;
}

/*
 * Provide the desktop component drag cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_component_drag_cancel(
    UmiDesktopComponentDragDrop *drag_drop,
    const char *session_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (drag_drop == NULL || session_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_entry(drag_drop, session_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (drag_drop->entries[index].snapshot.state ==
            UMI_DESKTOP_COMPONENT_DRAG_COMMITTED ||
        drag_drop->entries[index].snapshot.state ==
            UMI_DESKTOP_COMPONENT_DRAG_CANCELLED)
        return UMI_STATUS_INVALID_STATE;
    (void)mark_entry(
        drag_drop, &drag_drop->entries[index],
        UMI_DESKTOP_COMPONENT_DRAG_CANCELLED, 0, UMI_STATUS_CANCELLED);
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop component drag snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_component_drag_snapshot(
    const UmiDesktopComponentDragDrop *drag_drop,
    const char *session_id,
    UmiDesktopComponentDragSnapshot *out_snapshot)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (drag_drop == NULL || session_id == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_entry(drag_drop, session_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_snapshot = drag_drop->entries[index].snapshot;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by desktop component drag without changing
 * their state.
 */
size_t umi_desktop_component_drag_count(
    const UmiDesktopComponentDragDrop *drag_drop)
{
    return drag_drop != NULL ? drag_drop->count : 0U;
}
