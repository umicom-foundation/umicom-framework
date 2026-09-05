/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workbench_canvas.c
 *
 * PURPOSE:
 *   Implement the toolkit-neutral Workbench Canvas coordinator. The module
 *   delegates layout mutations to workspace customisation and owns only the
 *   host, monitor and detached-surface metadata needed by frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/workbench_canvas.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

/* Copy stable identifiers without silently truncating persisted values. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    int written;

    /* A null source or zero-sized destination cannot be used safely. */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(destination, capacity, "%s", source);
    /* Stable IDs must remain unique after copying, so truncation is rejected. */
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/* Find a mutable host once so every command applies the same identity rule. */
static UmiUiWorkbenchCanvasHost *find_host(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id)
{
    size_t index;
    size_t count;

    /* Missing inputs cannot identify a host. */
    if (canvas == NULL || host_id == NULL || host_id[0] == '\0') {
        return NULL;
    }
    /* The public capacity bounds this loop before every array access. */
    count = canvas->host_count > UMI_UI_WORKBENCH_CANVAS_MAX_HOSTS
        ? UMI_UI_WORKBENCH_CANVAS_MAX_HOSTS
        : canvas->host_count;
    for (index = 0U; index < count; ++index) {
        if (strcmp(canvas->hosts[index].host_id, host_id) == 0) {
            return &canvas->hosts[index];
        }
    }
    return NULL;
}

/* Find a host without allowing a const caller to mutate it. */
static const UmiUiWorkbenchCanvasHost *find_host_const(
    const UmiUiWorkbenchCanvas *canvas,
    const char *host_id)
{
    size_t index;
    size_t count;

    /* Missing inputs cannot identify a host. */
    if (canvas == NULL || host_id == NULL || host_id[0] == '\0') {
        return NULL;
    }
    /* The public capacity bounds this loop before every array access. */
    count = canvas->host_count > UMI_UI_WORKBENCH_CANVAS_MAX_HOSTS
        ? UMI_UI_WORKBENCH_CANVAS_MAX_HOSTS
        : canvas->host_count;
    for (index = 0U; index < count; ++index) {
        if (strcmp(canvas->hosts[index].host_id, host_id) == 0) {
            return &canvas->hosts[index];
        }
    }
    return NULL;
}

/* Find detached state for one window ID in a bounded host table. */
static UmiUiWorkbenchCanvasSurfaceState *find_surface_state(
    UmiUiWorkbenchCanvasHost *host,
    const char *window_id)
{
    size_t index;
    size_t count;

    /* Missing inputs cannot identify a surface. */
    if (host == NULL || window_id == NULL || window_id[0] == '\0') {
        return NULL;
    }
    /* Visit only registered records so malformed IDs cannot read outside the table. */
    count = host->surface_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS
        ? UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS
        : host->surface_count;
    for (index = 0U; index < count; ++index) {
        if (strcmp(host->surfaces[index].window_id, window_id) == 0) {
            return &host->surfaces[index];
        }
    }
    return NULL;
}

/* Keep monitor/detach records aligned with windows created by older APIs. */
static UmiStatus sync_surface_states(UmiUiWorkbenchCanvasHost *host)
{
    const UmiUiWorkspaceLayout *layout;
    size_t layout_index;
    size_t state_index;

    /* A host is usable only when its caller-owned customisation exists. */
    if (host == NULL || host->customisation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Reject a corrupted surface count before any bounded-table traversal. */
    if (host->surface_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    layout = umi_ui_workspace_customisation_active_const(host->customisation);
    /* An empty host is valid before its first layout is created. */
    if (layout == NULL) {
        host->surface_count = 0U;
        return UMI_STATUS_OK;
    }
    /* The layout model is caller-owned, so validate its bounded window count
     * before iterating the fixed window array. */
    if (layout->window_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Add state for windows created through the canonical layout API. */
    for (layout_index = 0U;
         layout_index < layout->window_count;
         ++layout_index) {
        const UmiUiWorkspaceWindow *window = &layout->windows[layout_index];
        UmiUiWorkbenchCanvasSurfaceState *state =
            find_surface_state(host, window->window_id);

        if (state != NULL) {
            state->visible = window->visible;
            continue;
        }
        /* Refuse to write outside the fixed table when a plugin misbehaves. */
        if (host->surface_count >= UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        state = &host->surfaces[host->surface_count];
        (void)memset(state, 0, sizeof(*state));
        if (copy_text(state->window_id,
                      sizeof(state->window_id),
                      window->window_id) != UMI_STATUS_OK ||
            copy_text(state->monitor_id,
                      sizeof(state->monitor_id),
                      host->monitor_id) != UMI_STATUS_OK) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        state->visible = window->visible;
        state->revision = 1U;
        host->surface_count += 1U;
    }
    /* Remove state records for windows closed through workspace customisation. */
    state_index = 0U;
    while (state_index < host->surface_count) {
        bool found = false;

        for (layout_index = 0U;
             layout_index < layout->window_count;
             ++layout_index) {
            if (strcmp(host->surfaces[state_index].window_id,
                       layout->windows[layout_index].window_id) == 0) {
                found = true;
                break;
            }
        }
        if (found) {
            state_index += 1U;
            continue;
        }
        (void)memmove(&host->surfaces[state_index],
                      &host->surfaces[state_index + 1U],
                      (host->surface_count - state_index - 1U) *
                          sizeof(host->surfaces[0]));
        host->surface_count -= 1U;
    }
    return UMI_STATUS_OK;
}

/* Begin a layout edit only after its host and model are available. */
static UmiStatus begin_host_edit(UmiUiWorkbenchCanvasHost *host)
{
    /* A null host or customisation cannot receive an edit command. */
    if (host == NULL || host->customisation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_ui_workspace_customisation_begin_edit(host->customisation);
}

/* Forward the rollback helper because commit validation may need to cancel. */
static UmiStatus cancel_host_edit(UmiUiWorkbenchCanvasHost *host);

/* Commit a successful edit and publish one host/canvas revision. */
static UmiStatus commit_host_edit(
    UmiUiWorkbenchCanvas *canvas,
    UmiUiWorkbenchCanvasHost *host)
{
    UmiStatus status;
    UmiUiWorkbenchCanvasSurfaceState surface_baseline[
        UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS];
    size_t surface_count_baseline;

    /* Both objects are required to publish a coherent change notification. */
    if (canvas == NULL || host == NULL || host->customisation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep detached-monitor metadata separate from the layout baseline.  A
     * cancelled edit can restore a removed window, and that window must regain
     * its previous monitor and revision instead of being recreated as docked. */
    if (host->surface_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    surface_count_baseline = host->surface_count;
    (void)memcpy(surface_baseline,
                 host->surfaces,
                 surface_count_baseline * sizeof(surface_baseline[0]));
    /* Synchronise while the edit baseline still exists.  If validation fails,
     * cancelling can restore both the layout and the surface registry. */
    status = sync_surface_states(host);
    if (status != UMI_STATUS_OK) {
        (void)cancel_host_edit(host);
        host->surface_count = surface_count_baseline;
        (void)memcpy(host->surfaces,
                     surface_baseline,
                     surface_count_baseline * sizeof(surface_baseline[0]));
        return status;
    }
    status = umi_ui_workspace_customisation_commit_edit(host->customisation);
    /* Do not publish a revision when the canonical model rejected the edit.
     * The edit remains open on that failure, so restore the captured baseline
     * before returning control to the caller. */
    if (status != UMI_STATUS_OK) {
        (void)cancel_host_edit(host);
        host->surface_count = surface_count_baseline;
        (void)memcpy(host->surfaces,
                     surface_baseline,
                     surface_count_baseline * sizeof(surface_baseline[0]));
        return status;
    }
    host->revision += 1U;
    canvas->revision += 1U;
    return UMI_STATUS_OK;
}

/* Cancel a partially applied command so callers never observe half a change. */
static UmiStatus cancel_host_edit(UmiUiWorkbenchCanvasHost *host)
{
    /* A null host has no edit session to cancel. */
    if (host == NULL || host->customisation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_ui_workspace_customisation_cancel_edit(host->customisation);
}

/* Round a normalised non-negative coordinate to the nearest grid line. */
static double snap_value(double value, double grid_step)
{
    const double scaled = value / grid_step;

    /* A very small persisted grid can overflow the old integer conversion.
     * Keep malformed arithmetic from producing an undefined conversion and let
     * the caller's existing geometry validation reject the result if needed. */
    if (!isfinite(scaled)) {
        return value;
    }
    return floor(scaled + 0.5) * grid_step;
}

/* Keep a snapped dimension inside the canvas and above a usable minimum. */
static double clamp_dimension(double value, double grid_step)
{
    const double minimum = grid_step > 0.05 ? grid_step : 0.05;

    if (value < minimum) {
        return minimum;
    }
    return value > 1.0 ? 1.0 : value;
}

/* Initialise an empty canvas before an application creates its first host. */
void umi_ui_workbench_canvas_init(UmiUiWorkbenchCanvas *canvas)
{
    /* A missing output object cannot be initialised safely. */
    if (canvas == NULL) {
        return;
    }
    (void)memset(canvas, 0, sizeof(*canvas));
}

/* Register one host while copying all identities into Framework-owned storage. */
UmiStatus umi_ui_workbench_canvas_add_host(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *application_id,
    const char *monitor_id,
    UmiUiWorkspaceCustomisation *customisation)
{
    UmiUiWorkbenchCanvasHost *host;
    UmiStatus status;
    size_t index;

    /* Every field participates in persistence and routing, so all are required. */
    if (canvas == NULL || host_id == NULL || application_id == NULL ||
        monitor_id == NULL || customisation == NULL || host_id[0] == '\0' ||
        application_id[0] == '\0' || monitor_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Stable host IDs prevent two native windows from receiving one command. */
    if (find_host(canvas, host_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Reject a host rather than writing outside the fixed host table. */
    if (canvas->host_count >= UMI_UI_WORKBENCH_CANVAS_MAX_HOSTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* A non-empty canvas must already have a routable active index.  Refuse a
     * corrupted snapshot instead of adding a host to an ambiguous session. */
    if (canvas->host_count > 0U &&
        canvas->active_host_index >= canvas->host_count) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Revisions are part of the observable contract; refuse a mutation that
     * would wrap the sequence number and confuse observers. */
    if (canvas->revision == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    host = &canvas->hosts[canvas->host_count];
    (void)memset(host, 0, sizeof(*host));
    if (copy_text(host->host_id, sizeof(host->host_id), host_id) !=
            UMI_STATUS_OK ||
        copy_text(host->application_id,
                  sizeof(host->application_id),
                  application_id) != UMI_STATUS_OK ||
        copy_text(host->monitor_id,
                  sizeof(host->monitor_id),
                  monitor_id) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    host->customisation = customisation;
    host->active = canvas->host_count == 0U;
    host->revision = 1U;
    status = sync_surface_states(host);
    if (status != UMI_STATUS_OK) {
        (void)memset(host, 0, sizeof(*host));
        return status;
    }
    canvas->host_count += 1U;
    if (host->active) {
        canvas->active_host_index = 0U;
    } else {
        /* Keep the active marker canonical when a caller supplied stale
         * boolean flags in an imported canvas snapshot. */
        for (index = 0U; index < canvas->host_count; ++index) {
            canvas->hosts[index].active =
                index == canvas->active_host_index;
        }
    }
    canvas->revision += 1U;
    return UMI_STATUS_OK;
}

/* Remove one host and repair the active-host index without freeing the
 * caller-owned customisation model attached to that host. */
UmiStatus umi_ui_workbench_canvas_remove_host(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id)
{
    size_t index;
    size_t count;
    size_t active_index;
    bool removed_active;

    /* A stable host ID is required to remove exactly one native window. */
    if (canvas == NULL || host_id == NULL || host_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (canvas->host_count > UMI_UI_WORKBENCH_CANVAS_MAX_HOSTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    count = canvas->host_count;
    if (count == 0U) return UMI_STATUS_NOT_FOUND;
    if (canvas->active_host_index >= count) {
        return UMI_STATUS_INVALID_STATE;
    }
    for (index = 0U; index < count; ++index) {
        if (strcmp(canvas->hosts[index].host_id, host_id) == 0) break;
    }
    /* Unknown hosts must not change the active selection or revision. */
    if (index == count) return UMI_STATUS_NOT_FOUND;
    /* Do not publish a wrapped revision when the host is removed. */
    if (canvas->revision == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    active_index = canvas->active_host_index;
    removed_active = index == active_index || canvas->hosts[index].active;
    if (index + 1U < count) {
        (void)memmove(&canvas->hosts[index],
                      &canvas->hosts[index + 1U],
                      (count - index - 1U) * sizeof(canvas->hosts[0]));
    }
    count -= 1U;
    (void)memset(&canvas->hosts[count], 0, sizeof(canvas->hosts[0]));
    canvas->host_count = count;

    if (count == 0U) {
        /* An empty canvas has no active host to route commands to. */
        canvas->active_host_index = 0U;
    } else if (removed_active) {
        /* Prefer the next host at the removed slot, or the final host when
         * the removed one was last. */
        if (active_index >= count) active_index = count - 1U;
        canvas->active_host_index = active_index;
        for (index = 0U; index < count; ++index) {
            canvas->hosts[index].active = index == active_index;
        }
    } else {
        /* Removing a host before the active one shifts its index left. */
        if (index < active_index) active_index -= 1U;
        canvas->active_host_index = active_index;
        /* Keep the boolean markers consistent even when a caller supplied a
         * previously inconsistent snapshot. */
        for (index = 0U; index < count; ++index) {
            canvas->hosts[index].active = index == active_index;
        }
    }
    canvas->revision += 1U;
    return UMI_STATUS_OK;
}

/* Select one host and clear the old active marker before publishing the new one. */
UmiStatus umi_ui_workbench_canvas_set_active_host(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id)
{
    UmiUiWorkbenchCanvasHost *host;
    size_t index;

    /* A host ID is required to route later surface commands. */
    if (canvas == NULL || host_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (canvas->host_count > UMI_UI_WORKBENCH_CANVAS_MAX_HOSTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    host = find_host(canvas, host_id);
    /* Unknown hosts cannot become active. */
    if (host == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    for (index = 0U; index < canvas->host_count; ++index) {
        canvas->hosts[index].active = false;
    }
    host->active = true;
    canvas->active_host_index = (size_t)(host - canvas->hosts);
    canvas->revision += 1U;
    return UMI_STATUS_OK;
}

/* Return a mutable registered host to command handlers. */
UmiUiWorkbenchCanvasHost *umi_ui_workbench_canvas_host(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id)
{
    return find_host(canvas, host_id);
}

/* Return an immutable registered host to renderers and inspectors. */
const UmiUiWorkbenchCanvasHost *umi_ui_workbench_canvas_host_const(
    const UmiUiWorkbenchCanvas *canvas,
    const char *host_id)
{
    return find_host_const(canvas, host_id);
}

/* Create an empty named layout through the existing customisation owner. */
UmiStatus umi_ui_workbench_canvas_create_blank_layout(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *layout_id,
    const char *name)
{
    UmiUiWorkbenchCanvasHost *host;
    UmiStatus status;

    /* Layout identity and display name are required for persistence and menus. */
    if (canvas == NULL || host_id == NULL || layout_id == NULL ||
        name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    host = find_host(canvas, host_id);
    /* Do not create a layout for an unknown native host. */
    if (host == NULL || host->customisation == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Use the atomic customisation operation rather than adding and activating
     * separately; a failed activation must not leave a hidden partial layout. */
    status = umi_ui_workspace_customisation_create_blank_layout(
        host->customisation, layout_id, name);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = sync_surface_states(host);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    host->revision += 1U;
    canvas->revision += 1U;
    return status;
}

/* Clear all removable windows in a rollback-safe edit session. */
UmiStatus umi_ui_workbench_canvas_clear_current_canvas(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id)
{
    UmiUiWorkbenchCanvasHost *host;
    UmiUiWorkspaceLayout *layout;
    UmiStatus status;

    /* A named host is required so clear cannot target the wrong native window. */
    if (canvas == NULL || host_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    host = find_host(canvas, host_id);
    /* Unknown hosts have no canvas to clear. */
    if (host == NULL || host->customisation == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = begin_host_edit(host);
    /* The edit lock protects the complete clear operation. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    layout = umi_ui_workspace_customisation_active(host->customisation);
    /* A host without an active layout cannot satisfy the clear operation. */
    if (layout == NULL) {
        (void)cancel_host_edit(host);
        return UMI_STATUS_NOT_FOUND;
    }
    if (layout->window_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS) {
        (void)cancel_host_edit(host);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    while (layout->window_count > 0U) {
        /* Index zero is safe because close compacts the remaining windows. */
        status = umi_ui_workspace_customisation_close_window(
            host->customisation, layout->windows[0].window_id);
        if (status != UMI_STATUS_OK) {
            (void)cancel_host_edit(host);
            return status;
        }
    }
    return commit_host_edit(canvas, host);
}

/* Open a catalogue surface using existing recent-window bookkeeping. */
UmiStatus umi_ui_workbench_canvas_open_surface(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *tool_id,
    const char *group_id,
    bool floating,
    uint64_t opened_at_ms,
    char *out_window_id,
    size_t out_window_id_capacity)
{
    UmiUiWorkbenchCanvasHost *host;
    UmiStatus status;

    /* All identity and output arguments are required by the delegated API. */
    if (canvas == NULL || host_id == NULL || tool_id == NULL ||
        group_id == NULL || out_window_id == NULL ||
        out_window_id_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    host = find_host(canvas, host_id);
    /* Opening on an unknown host would lose ownership context. */
    if (host == NULL || host->customisation == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = begin_host_edit(host);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_ui_workspace_customisation_open_window(
        host->customisation,
        tool_id,
        group_id,
        floating,
        opened_at_ms,
        out_window_id,
        out_window_id_capacity);
    /* An open failure must not leave an edit session locked open. */
    if (status != UMI_STATUS_OK) {
        (void)cancel_host_edit(host);
        return status;
    }
    return commit_host_edit(canvas, host);
}

/* Move a surface by converting it to a floating canvas window. */
UmiStatus umi_ui_workbench_canvas_move_surface(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *window_id,
    double x,
    double y)
{
    UmiUiWorkbenchCanvasHost *host;
    UmiUiWorkspaceLayout *layout;
    const UmiUiWorkspaceWindow *window;
    UmiUiWorkbenchCanvasSurfaceState *surface_state;
    UmiStatus status;

    /* The window ID and normalised position identify one safe edit. */
    if (canvas == NULL || host_id == NULL || window_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    host = find_host(canvas, host_id);
    if (host == NULL || host->customisation == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    layout = umi_ui_workspace_customisation_active(host->customisation);
    window = umi_ui_workspace_layout_find_window(layout, window_id);
    /* Do not begin an edit when the surface does not exist. */
    if (window == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* A detached native window has its own monitor geometry; it must be moved
     * through the frontend window manager rather than the canvas rectangle. */
    surface_state = find_surface_state(host, window_id);
    if (surface_state != NULL && surface_state->detached) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = begin_host_edit(host);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* A moved panel remains inside the Workbench canvas. The existing
     * float_window operation means a detached native window and must not be
     * used for an in-canvas gesture. */
    status = umi_ui_workspace_customisation_place_canvas_window(
        host->customisation, window_id, x, y, window->width, window->height);
    if (status != UMI_STATUS_OK) {
        (void)cancel_host_edit(host);
        return status;
    }
    return commit_host_edit(canvas, host);
}

/* Resize a surface while preserving its current origin. */
UmiStatus umi_ui_workbench_canvas_resize_surface(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *window_id,
    double width,
    double height)
{
    UmiUiWorkbenchCanvasHost *host;
    UmiUiWorkspaceLayout *layout;
    const UmiUiWorkspaceWindow *window;
    UmiUiWorkbenchCanvasSurfaceState *surface_state;
    UmiStatus status;

    /* The window ID and positive normalised dimensions are required. */
    if (canvas == NULL || host_id == NULL || window_id == NULL ||
        width <= 0.0 || height <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    host = find_host(canvas, host_id);
    if (host == NULL || host->customisation == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    layout = umi_ui_workspace_customisation_active(host->customisation);
    window = umi_ui_workspace_layout_find_window(layout, window_id);
    if (window == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Keep native detached-window bounds under the monitor/window manager; the
     * canvas API only changes surfaces owned by this host canvas. */
    surface_state = find_surface_state(host, window_id);
    if (surface_state != NULL && surface_state->detached) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = begin_host_edit(host);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Resize the semantic canvas rectangle without changing the detached
     * native-window flag used by the frontend host. */
    status = umi_ui_workspace_customisation_place_canvas_window(
        host->customisation, window_id, window->x, window->y, width, height);
    if (status != UMI_STATUS_OK) {
        (void)cancel_host_edit(host);
        return status;
    }
    return commit_host_edit(canvas, host);
}

/* Snap a floating surface and clamp its rectangle inside the normalised canvas. */
UmiStatus umi_ui_workbench_canvas_snap_surface(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *window_id,
    double grid_step)
{
    UmiUiWorkbenchCanvasHost *host;
    UmiUiWorkspaceLayout *layout;
    const UmiUiWorkspaceWindow *window;
    double x;
    double y;
    double width;
    double height;
    UmiStatus status;
    UmiUiWorkbenchCanvasSurfaceState *surface_state;

    /* A finite positive step avoids division by zero and invalid geometry. */
    if (canvas == NULL || host_id == NULL || window_id == NULL ||
        !isfinite(grid_step) || grid_step <= 0.0 || grid_step > 1.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    host = find_host(canvas, host_id);
    if (host == NULL || host->customisation == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    layout = umi_ui_workspace_customisation_active(host->customisation);
    window = umi_ui_workspace_layout_find_window(layout, window_id);
    /* Snapping a missing or docked surface would be ambiguous. */
    if (window == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (!window->floating &&
        strcmp(window->placement_id, UMI_UI_WORKSPACE_CANVAS_PLACEMENT) != 0) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Guard the floating-point-to-integer rounding below against malformed
     * persisted geometry; converting NaN or infinity to an integer is unsafe. */
    if (!isfinite(window->x) || !isfinite(window->y) ||
        !isfinite(window->width) || !isfinite(window->height) ||
        !isfinite(window->x + window->width) ||
        !isfinite(window->y + window->height) ||
        window->x < 0.0 || window->y < 0.0 ||
        window->width <= 0.0 || window->height <= 0.0 ||
        window->x > 1.0 || window->y > 1.0 ||
        window->width > 1.0 || window->height > 1.0) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* A detached native surface must be reattached before canvas snapping so
     * its monitor-specific window position is not silently discarded. */
    surface_state = find_surface_state(host, window_id);
    if (surface_state != NULL && surface_state->detached) {
        return UMI_STATUS_INVALID_STATE;
    }
    width = clamp_dimension(snap_value(window->width, grid_step), grid_step);
    height = clamp_dimension(snap_value(window->height, grid_step), grid_step);
    x = snap_value(window->x, grid_step);
    y = snap_value(window->y, grid_step);
    /* Keep right and bottom edges inside the host after rounding. */
    x = x + width > 1.0 ? 1.0 - width : x;
    y = y + height > 1.0 ? 1.0 - height : y;
    status = begin_host_edit(host);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Snapping keeps a panel in the canvas placement mode when it was already
     * moved there; explicit detach remains a separate monitor operation. */
    status = umi_ui_workspace_customisation_place_canvas_window(
        host->customisation, window_id, x, y, width, height);
    if (status != UMI_STATUS_OK) {
        (void)cancel_host_edit(host);
        return status;
    }
    return commit_host_edit(canvas, host);
}

/* Detach a surface after normalising it as a floating canvas window. */
UmiStatus umi_ui_workbench_canvas_detach_surface(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *window_id,
    const char *monitor_id)
{
    UmiUiWorkbenchCanvasHost *host;
    UmiUiWorkspaceLayout *layout;
    const UmiUiWorkspaceWindow *window;
    UmiUiWorkbenchCanvasSurfaceState *state;
    char validated_monitor_id[UMI_UI_WORKBENCH_CANVAS_MONITOR_ID_CAPACITY];
    UmiStatus status;

    /* Monitor identity is text so Windows, X11, Wayland and web frontends
     * can choose their own monitor-resolution mechanism. */
    if (canvas == NULL || host_id == NULL || window_id == NULL ||
        monitor_id == NULL || monitor_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Validate the monitor ID before changing the layout, so an oversized
     * monitor name cannot leave a detached panel with partial state. */
    status = copy_text(validated_monitor_id,
                       sizeof(validated_monitor_id),
                       monitor_id);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    host = find_host(canvas, host_id);
    if (host == NULL || host->customisation == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    layout = umi_ui_workspace_customisation_active(host->customisation);
    window = umi_ui_workspace_layout_find_window(layout, window_id);
    if (window == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = begin_host_edit(host);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (!window->floating) {
        /* Preserve current geometry while changing only placement mode. */
        status = umi_ui_workspace_customisation_float_window(
            host->customisation,
            window_id,
            window->x,
            window->y,
            window->width,
            window->height);
    } else {
        status = UMI_STATUS_OK;
    }
    if (status == UMI_STATUS_OK) {
        status = commit_host_edit(canvas, host);
    } else {
        (void)cancel_host_edit(host);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }
    state = find_surface_state(host, window_id);
    /* Synchronisation guarantees state for every active layout window. */
    if (state == NULL || copy_text(state->monitor_id,
                                  sizeof(state->monitor_id),
                                  validated_monitor_id) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    state->detached = true;
    state->revision += 1U;
    host->revision += 1U;
    canvas->revision += 1U;
    return UMI_STATUS_OK;
}

/* Reattach a detached surface to the canonical dock model. */
UmiStatus umi_ui_workbench_canvas_attach_surface(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *window_id,
    const char *placement_id,
    const char *stack_id)
{
    UmiUiWorkbenchCanvasHost *host;
    UmiUiWorkbenchCanvasSurfaceState *state;
    UmiStatus status;

    /* Placement and stack IDs are resolved by workspace customisation. */
    if (canvas == NULL || host_id == NULL || window_id == NULL ||
        placement_id == NULL || stack_id == NULL ||
        placement_id[0] == '\0' || stack_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    host = find_host(canvas, host_id);
    if (host == NULL || host->customisation == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    state = find_surface_state(host, window_id);
    if (state == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = begin_host_edit(host);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_ui_workspace_customisation_dock_window(
        host->customisation, window_id, placement_id, stack_id);
    if (status != UMI_STATUS_OK) {
        (void)cancel_host_edit(host);
        return status;
    }
    status = commit_host_edit(canvas, host);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    state = find_surface_state(host, window_id);
    if (state == NULL || copy_text(state->monitor_id,
                                  sizeof(state->monitor_id),
                                  host->monitor_id) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    state->detached = false;
    state->revision += 1U;
    host->revision += 1U;
    canvas->revision += 1U;
    return UMI_STATUS_OK;
}

/* Lock or unlock the active layout through the one existing layout owner. */
UmiStatus umi_ui_workbench_canvas_set_layout_locked(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    bool locked)
{
    UmiUiWorkbenchCanvasHost *host;
    UmiUiWorkspaceLayout *layout;
    UmiStatus status;

    /* A named host prevents changing another monitor's layout. */
    if (canvas == NULL || host_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    host = find_host(canvas, host_id);
    if (host == NULL || host->customisation == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    layout = umi_ui_workspace_customisation_active(host->customisation);
    if (layout == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = umi_ui_workspace_layout_set_locked(layout, locked);
    /* Only successful lock changes become visible in host snapshots. */
    if (status == UMI_STATUS_OK) {
        host->revision += 1U;
        canvas->revision += 1U;
    }
    return status;
}

/* Return detached state without transferring ownership to the caller. */
const UmiUiWorkbenchCanvasSurfaceState *umi_ui_workbench_canvas_surface_state(
    const UmiUiWorkbenchCanvasHost *host,
    const char *window_id)
{
    size_t index;
    size_t count;

    /* Missing inputs cannot identify a borrowed state record. */
    if (host == NULL || window_id == NULL) {
        return NULL;
    }
    count = host->surface_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS
        ? UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS
        : host->surface_count;
    for (index = 0U; index < count; ++index) {
        if (strcmp(host->surfaces[index].window_id, window_id) == 0) {
            return &host->surfaces[index];
        }
    }
    return NULL;
}

/* Build an immutable summary so frontends do not inspect mutable state during a command. */
UmiStatus umi_ui_workbench_canvas_snapshot(
    const UmiUiWorkbenchCanvas *canvas,
    UmiUiWorkbenchCanvasSnapshot *out_snapshot)
{
    size_t index;

    /* Both pointers are required for a meaningful snapshot. */
    if (canvas == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Refuse a malformed host count before copying into the fixed snapshot. */
    if (canvas->host_count > UMI_UI_WORKBENCH_CANVAS_MAX_HOSTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* A non-empty session must point at a real host so consumers can safely
     * use active_host_index without another bounds check. */
    if (canvas->host_count > 0U &&
        canvas->active_host_index >= canvas->host_count) {
        return UMI_STATUS_INVALID_STATE;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->host_count = canvas->host_count;
    out_snapshot->active_host_index = canvas->active_host_index;
    out_snapshot->revision = canvas->revision;
    for (index = 0U; index < canvas->host_count; ++index) {
        const UmiUiWorkbenchCanvasHost *host = &canvas->hosts[index];
        UmiUiWorkbenchCanvasHostSnapshot *summary = &out_snapshot->hosts[index];
        const UmiUiWorkspaceLayout *layout = host->customisation != NULL
            ? umi_ui_workspace_customisation_active_const(host->customisation)
            : NULL;

        if (host->surface_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        (void)snprintf(summary->host_id,
                       sizeof(summary->host_id),
                       "%s",
                       host->host_id);
        (void)snprintf(summary->application_id,
                       sizeof(summary->application_id),
                       "%s",
                       host->application_id);
        (void)snprintf(summary->monitor_id,
                       sizeof(summary->monitor_id),
                       "%s",
                       host->monitor_id);
        if (layout != NULL) {
            (void)snprintf(summary->active_layout_id,
                           sizeof(summary->active_layout_id),
                           "%s",
                           layout->layout_id);
            summary->layout_locked = layout->locked;
        }
        summary->surface_count = host->surface_count;
        summary->active = host->active;
        summary->revision = host->revision;
    }
    return UMI_STATUS_OK;
}
