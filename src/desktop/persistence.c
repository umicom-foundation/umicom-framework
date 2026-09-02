/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/persistence.c
 *
 * PURPOSE:
 *   Implement the persistence behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework | Federated desktop persistence
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/persistence.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_DESKTOP_PERSISTENCE_LINE_CAPACITY 2048U
#define UMI_DESKTOP_PERSISTENCE_MAX_FIELDS 16U

typedef struct UmiDesktopTextWriter {
    char *text;
    size_t capacity;
    size_t length;
} UmiDesktopTextWriter;

/* Provide the safe text operation used by this module and its client applications. */
static bool safe_text(const char *text)
{
    return text != NULL && text[0] != '\0' && strchr(text, '|') == NULL &&
        strchr(text, '\n') == NULL && strchr(text, '\r') == NULL;
}

/* Provide the append text operation used by this module and its client applications. */
static UmiStatus append_text(UmiDesktopTextWriter *writer,
                             const char *format, ...)
{
    va_list arguments;
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (writer == NULL || format == NULL || writer->length >= writer->capacity)
        return UMI_STATUS_INVALID_ARGUMENT;
    va_start(arguments, format);
    written = vsnprintf(writer->text + writer->length,
                        writer->capacity - writer->length, format, arguments);
    va_end(arguments);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (written < 0) return UMI_STATUS_IO_ERROR;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if ((size_t)written >= writer->capacity - writer->length)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    writer->length += (size_t)written;
    return UMI_STATUS_OK;
}

/* Provide the split fields operation used by this module and its client applications. */
static size_t split_fields(char *line, char **fields, size_t capacity)
{
    size_t count = 0U;
    char *cursor = line;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (line == NULL || fields == NULL || capacity == 0U) return 0U;
    fields[count++] = cursor;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '|') {
            *cursor = '\0';
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (count >= capacity) return capacity + 1U;
            fields[count++] = cursor + 1;
        }
        cursor += 1;
    }
    return count;
}

/* Provide the parse int32 operation used by this module and its client applications. */
static UmiStatus parse_int32(const char *text, int32_t *out_value)
{
    char *end = NULL;
    long value;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    errno = 0;
    value = strtol(text, &end, 10);
    /* Apply this branch only when its contract condition is satisfied. */
    if (errno != 0 || end == text || *end != '\0' ||
        value < INT32_MIN || value > INT32_MAX)
        return UMI_STATUS_PARSE_ERROR;
    *out_value = (int32_t)value;
    return UMI_STATUS_OK;
}

/* Provide the parse uint32 operation used by this module and its client applications. */
static UmiStatus parse_uint32(const char *text, uint32_t *out_value)
{
    char *end = NULL;
    unsigned long value;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    errno = 0;
    value = strtoul(text, &end, 10);
    /* Apply this branch only when its contract condition is satisfied. */
    if (errno != 0 || end == text || *end != '\0' || value > UINT32_MAX)
        return UMI_STATUS_PARSE_ERROR;
    *out_value = (uint32_t)value;
    return UMI_STATUS_OK;
}

/*
 * Provide the parse double value operation used by this module and its client
 * applications.
 */
static UmiStatus parse_double_value(const char *text, double *out_value)
{
    char *end = NULL;
    double value;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    errno = 0;
    value = strtod(text, &end);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (errno != 0 || end == text || *end != '\0' || value <= 0.0)
        return UMI_STATUS_PARSE_ERROR;
    *out_value = value;
    return UMI_STATUS_OK;
}

/* Provide the copy field operation used by this module and its client applications. */
static UmiStatus copy_field(char *destination, size_t capacity,
                            const char *field)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || !safe_text(field))
        return UMI_STATUS_PARSE_ERROR;
    length = strlen(field);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, field, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Write desktop persistence in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_desktop_persistence_encode(
    UmiDesktopRuntime *runtime,
    char *out_text,
    size_t capacity,
    size_t *out_length)
{
    UmiDesktopMonitorTopology *monitors;
    UmiDesktopLayoutTabs *tabs;
    UmiDesktopLayoutCatalogue *layouts;
    UmiDesktopWindowManager *windows;
    UmiDesktopTextWriter writer;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || out_text == NULL || capacity == 0U ||
        out_length == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    monitors = umi_desktop_runtime_monitors(runtime);
    tabs = umi_desktop_runtime_tabs(runtime);
    layouts = umi_desktop_runtime_layouts(runtime);
    windows = umi_desktop_runtime_windows(runtime);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (monitors == NULL || tabs == NULL || layouts == NULL || windows == NULL)
        return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!safe_text(layouts->active_layout_id)) return UMI_STATUS_INVALID_STATE;
    writer = (UmiDesktopTextWriter){out_text, capacity, 0U};
    out_text[0] = '\0';
    status = append_text(&writer, "UMICOM-DESKTOP|1\nACTIVE|%s\n",
                         layouts->active_layout_id);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < monitors->count;
         ++index) {
        const UmiDesktopMonitor *monitor = &monitors->monitors[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!safe_text(monitor->monitor_id) || !safe_text(monitor->name))
            return UMI_STATUS_INVALID_STATE;
        status = append_text(&writer,
            "MONITOR|%s|%s|%d|%d|%d|%d|%d|%d|%d|%d|%.6f|%.6f|%d|%d\n",
            monitor->monitor_id, monitor->name,
            monitor->bounds.x, monitor->bounds.y, monitor->bounds.width,
            monitor->bounds.height, monitor->work_area.x,
            monitor->work_area.y, monitor->work_area.width,
            monitor->work_area.height, monitor->scale,
            monitor->refresh_rate_hz, monitor->primary ? 1 : 0,
            monitor->enabled ? 1 : 0);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < tabs->count; ++index) {
        const UmiDesktopLayoutTab *tab = &tabs->tabs[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!safe_text(tab->tab_id) || !safe_text(tab->layout_id) ||
            !safe_text(tab->label)) return UMI_STATUS_INVALID_STATE;
        status = append_text(&writer,
            "TAB|%s|%s|%s|%u|%d|%d|%d|%d\n", tab->tab_id,
            tab->layout_id, tab->label, tab->order, tab->active ? 1 : 0,
            tab->pinned ? 1 : 0, tab->dirty ? 1 : 0,
            tab->closable ? 1 : 0);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < windows->count;
         ++index) {
        const UmiDesktopWindow *window = &windows->windows[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!safe_text(window->window_id) || !safe_text(window->monitor_id))
            return UMI_STATUS_INVALID_STATE;
        status = append_text(&writer,
            "WINDOW|%s|%s|%d|%d|%d|%d|%d|%d|%d\n",
            window->window_id, window->monitor_id, window->bounds.x,
            window->bounds.y, window->bounds.width, window->bounds.height,
            (int)window->dock_placement, window->visible ? 1 : 0,
            window->maximised ? 1 : 0);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_text(&writer, "END\n");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    *out_length = writer.length;
    return UMI_STATUS_OK;
}

/* Provide the decode monitor operation used by this module and its client applications. */
static UmiStatus decode_monitor(
    UmiDesktopRuntime *runtime,
    char **fields,
    size_t count)
{
    UmiDesktopMonitor monitor;
    UmiDesktopMonitorTopology *topology = umi_desktop_runtime_monitors(runtime);
    uint32_t primary;
    uint32_t enabled;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (count != 15U || topology == NULL) return UMI_STATUS_PARSE_ERROR;
    (void)memset(&monitor, 0, sizeof(monitor));
    status = copy_field(monitor.monitor_id, sizeof(monitor.monitor_id), fields[1]);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_field(monitor.name, sizeof(monitor.name), fields[2]);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_int32(fields[3], &monitor.bounds.x);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_int32(fields[4], &monitor.bounds.y);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_int32(fields[5], &monitor.bounds.width);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_int32(fields[6], &monitor.bounds.height);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_int32(fields[7], &monitor.work_area.x);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_int32(fields[8], &monitor.work_area.y);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_int32(fields[9], &monitor.work_area.width);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_int32(fields[10], &monitor.work_area.height);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_double_value(fields[11], &monitor.scale);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_double_value(fields[12], &monitor.refresh_rate_hz);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_uint32(fields[13], &primary);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_uint32(fields[14], &enabled);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || primary > 1U || enabled > 1U)
        return UMI_STATUS_PARSE_ERROR;
    monitor.primary = primary == 1U;
    monitor.enabled = enabled == 1U;
    return umi_desktop_monitor_topology_find(topology, monitor.monitor_id) != NULL
        ? umi_desktop_monitor_topology_update(topology, &monitor)
        : umi_desktop_monitor_topology_add(topology, &monitor);
}

/* Provide the decode tab operation used by this module and its client applications. */
static UmiStatus decode_tab(
    UmiDesktopRuntime *runtime,
    char **fields,
    size_t count)
{
    UmiDesktopLayoutTabs *tabs = umi_desktop_runtime_tabs(runtime);
    const UmiDesktopLayoutTab *existing;
    uint32_t order;
    uint32_t active;
    uint32_t pinned;
    uint32_t dirty;
    uint32_t closable;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (count != 9U || tabs == NULL) return UMI_STATUS_PARSE_ERROR;
    existing = umi_desktop_layout_tabs_find(tabs, fields[1]);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (existing == NULL || strcmp(existing->layout_id, fields[2]) != 0)
        return UMI_STATUS_NOT_FOUND;
    status = parse_uint32(fields[4], &order);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_uint32(fields[5], &active);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_uint32(fields[6], &pinned);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_uint32(fields[7], &dirty);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_uint32(fields[8], &closable);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || active > 1U || pinned > 1U || dirty > 1U ||
        closable > 1U || order >= tabs->count) return UMI_STATUS_PARSE_ERROR;
    status = umi_desktop_layout_tabs_rename(tabs, fields[1], fields[3]);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_desktop_layout_tabs_move(tabs, fields[1], order);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_desktop_layout_tabs_set_dirty(tabs, fields[1], dirty == 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && active == 1U)
        status = umi_desktop_layout_tabs_activate(tabs, fields[1]);
    return status;
}

/* Provide the decode window operation used by this module and its client applications. */
static UmiStatus decode_window(
    UmiDesktopRuntime *runtime,
    char **fields,
    size_t count)
{
    UmiDesktopRect bounds;
    int32_t placement_value;
    uint32_t visible;
    uint32_t maximised;
    UmiStatus status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count != 10U)
        return UMI_STATUS_PARSE_ERROR;
    status = parse_int32(fields[3], &bounds.x);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_int32(fields[4], &bounds.y);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_int32(fields[5], &bounds.width);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_int32(fields[6], &bounds.height);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_int32(fields[7], &placement_value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_uint32(fields[8], &visible);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = parse_uint32(fields[9], &maximised);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || visible > 1U || maximised > 1U ||
        placement_value < (int32_t)UMI_DESKTOP_DOCK_CANVAS ||
        placement_value > (int32_t)UMI_DESKTOP_DOCK_FLOATING)
        return UMI_STATUS_PARSE_ERROR;
    return umi_desktop_runtime_restore_window_session(
        runtime, fields[1], fields[2], bounds,
        (UmiDesktopDockPlacement)placement_value, visible == 1U,
        maximised == 1U);
}

/*
 * Read desktop persistence into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_desktop_persistence_decode(
    UmiDesktopRuntime *runtime,
    const char *text)
{
    const char *cursor;
    bool header_seen = false;
    bool active_seen = false;
    bool end_seen = false;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || text == NULL || text[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    cursor = text;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        char line[UMI_DESKTOP_PERSISTENCE_LINE_CAPACITY];
        char *fields[UMI_DESKTOP_PERSISTENCE_MAX_FIELDS];
        const char *newline = strchr(cursor, '\n');
        const size_t length = newline != NULL ? (size_t)(newline - cursor)
                                              : strlen(cursor);
        size_t count;
        UmiStatus status = UMI_STATUS_OK;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length == 0U) {
            cursor = newline != NULL ? newline + 1 : cursor + length;
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= sizeof(line)) return UMI_STATUS_CAPACITY_EXCEEDED;
        (void)memcpy(line, cursor, length);
        line[length] = '\0';
        count = split_fields(line, fields,
                             sizeof(fields) / sizeof(fields[0]));
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (count == 0U || count > sizeof(fields) / sizeof(fields[0]))
            return UMI_STATUS_PARSE_ERROR;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!header_seen) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (count != 2U || strcmp(fields[0], "UMICOM-DESKTOP") != 0 ||
                strcmp(fields[1], "1") != 0) return UMI_STATUS_PARSE_ERROR;
            header_seen = true;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(fields[0], "ACTIVE") == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (count != 2U || active_seen) return UMI_STATUS_PARSE_ERROR;
            status = umi_desktop_runtime_activate_layout(runtime, fields[1]);
            active_seen = status == UMI_STATUS_OK;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(fields[0], "MONITOR") == 0) {
            status = decode_monitor(runtime, fields, count);
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(fields[0], "TAB") == 0) {
            status = decode_tab(runtime, fields, count);
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(fields[0], "WINDOW") == 0) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (!active_seen) return UMI_STATUS_PARSE_ERROR;
            status = decode_window(runtime, fields, count);
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(fields[0], "END") == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (count != 1U) return UMI_STATUS_PARSE_ERROR;
            end_seen = true;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            return UMI_STATUS_PARSE_ERROR;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end_seen && newline != NULL && newline[1] != '\0')
            return UMI_STATUS_PARSE_ERROR;
        cursor = newline != NULL ? newline + 1 : cursor + length;
    }
    return header_seen && active_seen && end_seen
        ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

/*
 * Provide the desktop persistence decode transactional operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_persistence_decode_transactional(
    UmiDesktopRuntime *runtime,
    const char *text)
{
    UmiDesktopRuntimeState *state;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || text == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    state = (UmiDesktopRuntimeState *)calloc(1U, sizeof(*state));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_desktop_runtime_capture_state(runtime, state);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(state);
        return status;
    }
    status = umi_desktop_persistence_decode(runtime, text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        const UmiStatus restore_status =
            umi_desktop_runtime_restore_state(runtime, state);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (restore_status != UMI_STATUS_OK)
            status = restore_status;
    }
    free(state);
    return status;
}
