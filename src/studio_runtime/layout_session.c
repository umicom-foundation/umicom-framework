/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/layout_session.c
 *
 * PURPOSE:
 *   Implement line-safe semantic shell layout/session persistence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/layout_session.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiStatus key(
    const char *prefix,
    const char *suffix,
    char *out,
    size_t capacity)
{
    int written = snprintf(out, capacity, "%s.%s", prefix, suffix);
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

static UmiStatus set_number(
    UmiSessionStore *store,
    const char *prefix,
    const char *suffix,
    uint64_t value)
{
    char k[UMI_SESSION_KEY_CAPACITY];
    char v[32];
    int written;
    UmiStatus status = key(prefix, suffix, k, sizeof(k));

    if (status != UMI_STATUS_OK) return status;

    written = snprintf(v, sizeof(v), "%llu", (unsigned long long)value);
    if (written < 0 || (size_t)written >= sizeof(v)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    return umi_session_store_set(store, k, v);
}

static UmiStatus get_number(
    const UmiSessionStore *store,
    const char *prefix,
    const char *suffix,
    uint64_t default_value,
    uint64_t *out_value)
{
    char k[UMI_SESSION_KEY_CAPACITY];
    char v[32];
    char *end = NULL;
    unsigned long long parsed;
    UmiStatus status = key(prefix, suffix, k, sizeof(k));

    if (status != UMI_STATUS_OK) return status;

    status = umi_session_store_get(store, k, v, sizeof(v));
    if (status == UMI_STATUS_NOT_FOUND) {
        *out_value = default_value;
        return UMI_STATUS_OK;
    }
    if (status != UMI_STATUS_OK) return status;

    parsed = strtoull(v, &end, 10);
    if (end == v || *end != '\0') return UMI_STATUS_PARSE_ERROR;

    *out_value = (uint64_t)parsed;
    return UMI_STATUS_OK;
}

static UmiStatus set_text(
    UmiSessionStore *store,
    const char *prefix,
    const char *suffix,
    const char *value)
{
    char k[UMI_SESSION_KEY_CAPACITY];
    UmiStatus status = key(prefix, suffix, k, sizeof(k));
    if (status != UMI_STATUS_OK) return status;
    return umi_session_store_set(store, k, value);
}

static UmiStatus get_text(
    const UmiSessionStore *store,
    const char *prefix,
    const char *suffix,
    char *out,
    size_t capacity)
{
    char k[UMI_SESSION_KEY_CAPACITY];
    UmiStatus status = key(prefix, suffix, k, sizeof(k));
    if (status != UMI_STATUS_OK) return status;
    return umi_session_store_get(store, k, out, capacity);
}

UmiStatus umi_studio_layout_session_save(
    UmiSessionStore *store,
    const char *prefix,
    const char *active_preset_id,
    const UmiApplicationShellState *state,
    const UmiApplicationShellLayout *layout)
{
    size_t index;
    UmiStatus status;

    if (store == NULL || prefix == NULL ||
        active_preset_id == NULL || state == NULL || layout == NULL ||
        layout->placement_count > UMI_STUDIO_LAYOUT_SESSION_MAX_PLACEMENTS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = set_text(store, prefix, "preset", active_preset_id);
    if (status == UMI_STATUS_OK)
        status = set_text(store, prefix, "activity", state->active_activity_id);
    if (status == UMI_STATUS_OK)
        status = set_text(store, prefix, "view", state->active_view_id);
    if (status == UMI_STATUS_OK)
        status = set_text(store, prefix, "focus", state->focused_view_id);
    if (status == UMI_STATUS_OK)
        status = set_text(
            store, prefix, "perspective", state->active_perspective_id);
    if (status == UMI_STATUS_OK)
        status = set_text(store, prefix, "layoutId", layout->layout_id);
    if (status == UMI_STATUS_OK)
        status = set_text(store, prefix, "layoutTitle", layout->title);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, prefix, "primary", state->primary_sidebar_visible ? 1U : 0U);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, prefix, "secondary", state->secondary_sidebar_visible ? 1U : 0U);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, prefix, "bottom", state->bottom_panel_visible ? 1U : 0U);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, prefix, "status", state->status_bar_visible ? 1U : 0U);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, prefix, "zen", state->zen_mode ? 1U : 0U);
    if (status == UMI_STATUS_OK)
        status = set_number(
            store, prefix, "placementCount", layout->placement_count);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < layout->placement_count; ++index) {
        const UmiApplicationShellPlacement *placement =
            &layout->placements[index];
        char p[UMI_SESSION_KEY_CAPACITY];
        int written = snprintf(p, sizeof(p), "%s.p%zu", prefix, index);

        if (written < 0 || (size_t)written >= sizeof(p)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        status = set_text(
            store, p, "id", placement->contribution_id);
        if (status == UMI_STATUS_OK)
            status = set_text(
                store, p, "container", placement->container_id);
        if (status == UMI_STATUS_OK)
            status = set_number(
                store, p, "region", (uint64_t)placement->region);
        if (status == UMI_STATUS_OK)
            status = set_number(
                store, p, "order", placement->order);
        if (status == UMI_STATUS_OK)
            status = set_number(
                store, p, "visible", placement->visible ? 1U : 0U);
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_studio_layout_session_restore(
    const UmiSessionStore *store,
    const char *prefix,
    char *out_preset_id,
    size_t preset_capacity,
    UmiApplicationShellState *state,
    UmiApplicationShellLayout *layout,
    int *out_restored)
{
    char layout_id[UMI_APPLICATION_SHELL_ID_CAPACITY];
    char layout_title[UMI_APPLICATION_SHELL_TITLE_CAPACITY];
    uint64_t count = 0U;
    uint64_t value = 0U;
    size_t index;
    UmiStatus status;

    if (store == NULL || prefix == NULL ||
        out_preset_id == NULL || preset_capacity == 0U ||
        state == NULL || layout == NULL || out_restored == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_restored = 0;

    status = get_text(
        store, prefix, "preset", out_preset_id, preset_capacity);
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;

    status = get_text(
        store, prefix, "activity",
        state->active_activity_id, sizeof(state->active_activity_id));
    if (status == UMI_STATUS_OK)
        status = get_text(
            store, prefix, "view",
            state->active_view_id, sizeof(state->active_view_id));
    if (status == UMI_STATUS_OK)
        status = get_text(
            store, prefix, "focus",
            state->focused_view_id, sizeof(state->focused_view_id));
    if (status == UMI_STATUS_OK)
        status = get_text(
            store, prefix, "perspective",
            state->active_perspective_id, sizeof(state->active_perspective_id));
    if (status == UMI_STATUS_OK)
        status = get_text(
            store, prefix, "layoutId", layout_id, sizeof(layout_id));
    if (status == UMI_STATUS_OK)
        status = get_text(
            store, prefix, "layoutTitle", layout_title, sizeof(layout_title));
    if (status != UMI_STATUS_OK) return status;

#define GET_BOOL(name, target) \
    do { \
        status = get_number(store, prefix, name, 0U, &value); \
        if (status != UMI_STATUS_OK || value > 1U) { \
            return status != UMI_STATUS_OK ? status : UMI_STATUS_PARSE_ERROR; \
        } \
        target = value != 0U; \
    } while (0)

    GET_BOOL("primary", state->primary_sidebar_visible);
    GET_BOOL("secondary", state->secondary_sidebar_visible);
    GET_BOOL("bottom", state->bottom_panel_visible);
    GET_BOOL("status", state->status_bar_visible);
    GET_BOOL("zen", state->zen_mode);

#undef GET_BOOL

    status = get_number(store, prefix, "placementCount", 0U, &count);
    if (status != UMI_STATUS_OK ||
        count > UMI_STUDIO_LAYOUT_SESSION_MAX_PLACEMENTS) {
        return status != UMI_STATUS_OK
            ? status
            : UMI_STATUS_PARSE_ERROR;
    }

    umi_application_shell_layout_init(layout, layout_id, layout_title);

    for (index = 0U; index < (size_t)count; ++index) {
        char p[UMI_SESSION_KEY_CAPACITY];
        char id[UMI_APPLICATION_SHELL_ID_CAPACITY];
        char container[UMI_APPLICATION_SHELL_ID_CAPACITY];
        uint64_t region = 0U;
        uint64_t order = 0U;
        uint64_t visible = 0U;
        int written = snprintf(p, sizeof(p), "%s.p%zu", prefix, index);

        if (written < 0 || (size_t)written >= sizeof(p)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        status = get_text(store, p, "id", id, sizeof(id));
        if (status == UMI_STATUS_OK)
            status = get_text(
                store, p, "container", container, sizeof(container));
        if (status == UMI_STATUS_OK)
            status = get_number(store, p, "region", 0U, &region);
        if (status == UMI_STATUS_OK)
            status = get_number(store, p, "order", 0U, &order);
        if (status == UMI_STATUS_OK)
            status = get_number(store, p, "visible", 0U, &visible);
        if (status != UMI_STATUS_OK ||
            region > UMI_APPLICATION_SHELL_REGION_WINDOW ||
            order > SIZE_MAX ||
            visible > 1U) {
            return status != UMI_STATUS_OK
                ? status
                : UMI_STATUS_PARSE_ERROR;
        }

        status = umi_application_shell_layout_place(
            layout,
            id,
            container,
            (UmiApplicationShellRegion)region,
            (size_t)order,
            visible != 0U);
        if (status != UMI_STATUS_OK) return status;
    }

    state->revision += 1U;
    *out_restored = 1;
    return UMI_STATUS_OK;
}
