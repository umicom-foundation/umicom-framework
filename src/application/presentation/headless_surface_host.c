/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/headless_surface_host.c
 *
 * PURPOSE:
 *   Implement a deterministic in-memory host for portable application panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/headless_surface_host.h"

#include <string.h>

static UmiApplicationPresentationHeadlessPanel *find_mutable(
    UmiApplicationPresentationHeadlessSurfaceHost *host,
    const char *component_id)
{
    size_t index;
    for (index = 0U; index < host->panel_count; ++index) {
        if (strcmp(host->panels[index].component_id, component_id) == 0) {
            return &host->panels[index];
        }
    }
    return NULL;
}

static UmiStatus headless_begin(
    void *context,
    const UmiApplicationPresentationWindowSpec *window)
{
    UmiApplicationPresentationHeadlessSurfaceHost *host = context;
    if (host == NULL || window == NULL || host->active) {
        return UMI_STATUS_INVALID_STATE;
    }
    host->window_id = window->window_id;
    host->active = 1;
    host->event_count += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus headless_mount(
    void *context,
    const UmiApplicationPresentationSurfaceItem *item)
{
    UmiApplicationPresentationHeadlessSurfaceHost *host = context;
    UmiApplicationPresentationHeadlessPanel *panel;
    if (host == NULL || item == NULL || !host->active) {
        return UMI_STATUS_INVALID_STATE;
    }
    panel = find_mutable(host, item->placement->panel->component_id);
    if (panel == NULL) {
        if (host->panel_count >= UMI_APPLICATION_PRESENTATION_PLAN_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        panel = &host->panels[host->panel_count++];
        panel->component_id = item->placement->panel->component_id;
    }
    panel->state = item->state;
    panel->visible = 1;
    panel->focused = item->focused;
    panel->revision = item->revision;
    host->event_count += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus headless_update(
    void *context,
    const UmiApplicationPresentationSurfaceItem *item)
{
    UmiApplicationPresentationHeadlessSurfaceHost *host = context;
    UmiApplicationPresentationHeadlessPanel *panel;
    if (host == NULL || item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    panel = find_mutable(host, item->placement->panel->component_id);
    if (panel == NULL) return UMI_STATUS_NOT_FOUND;
    panel->state = item->state;
    panel->visible = item->visible;
    panel->focused = item->focused;
    panel->revision = item->revision;
    host->event_count += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus headless_focus(
    void *context,
    const UmiApplicationPresentationSurfaceItem *item)
{
    UmiApplicationPresentationHeadlessSurfaceHost *host = context;
    size_t index;
    if (host == NULL || item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    host->focused_component_id = item->placement->panel->component_id;
    for (index = 0U; index < host->panel_count; ++index) {
        host->panels[index].focused =
            strcmp(host->panels[index].component_id,
                   host->focused_component_id) == 0;
    }
    host->event_count += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus headless_unmount(
    void *context,
    const UmiApplicationPresentationSurfaceItem *item)
{
    UmiApplicationPresentationHeadlessSurfaceHost *host = context;
    UmiApplicationPresentationHeadlessPanel *panel;
    if (host == NULL || item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    panel = find_mutable(host, item->placement->panel->component_id);
    if (panel == NULL) return UMI_STATUS_NOT_FOUND;
    panel->visible = 0;
    panel->focused = 0;
    panel->state = UMI_APPLICATION_PRESENTATION_STATE_DORMANT;
    if (host->focused_component_id != NULL &&
        strcmp(host->focused_component_id, panel->component_id) == 0) {
        host->focused_component_id = NULL;
    }
    host->event_count += 1U;
    return UMI_STATUS_OK;
}

static void headless_end(void *context)
{
    UmiApplicationPresentationHeadlessSurfaceHost *host = context;
    if (host != NULL) {
        host->active = 0;
        host->event_count += 1U;
    }
}

static const UmiApplicationPresentationSurfaceHostOperations OPERATIONS = {
    sizeof(UmiApplicationPresentationSurfaceHostOperations),
    headless_begin,
    headless_mount,
    headless_update,
    headless_focus,
    headless_unmount,
    headless_end};

void umi_application_presentation_headless_surface_host_init(
    UmiApplicationPresentationHeadlessSurfaceHost *host)
{
    if (host == NULL) return;
    (void)memset(host, 0, sizeof(*host));
    host->host.operations = &OPERATIONS;
    host->host.context = host;
}

UmiApplicationPresentationSurfaceHost *
umi_application_presentation_headless_surface_host_interface(
    UmiApplicationPresentationHeadlessSurfaceHost *host)
{
    return host != NULL ? &host->host : NULL;
}

const UmiApplicationPresentationHeadlessPanel *
umi_application_presentation_headless_surface_host_find(
    const UmiApplicationPresentationHeadlessSurfaceHost *host,
    const char *component_id)
{
    size_t index;
    if (host == NULL || component_id == NULL) return NULL;
    for (index = 0U; index < host->panel_count; ++index) {
        if (strcmp(host->panels[index].component_id, component_id) == 0) {
            return &host->panels[index];
        }
    }
    return NULL;
}
