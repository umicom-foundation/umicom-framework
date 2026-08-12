/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workbench_state.c
 *
 * PURPOSE:
 *   Implement a compact, deterministic text representation of workbench
 *   navigation and chrome state, including empty optional identifiers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The encoded form is intentionally simple so it can be stored by the existing
 * Session Store without adding another persistence engine. The decoder does
 * not use strtok() because empty fields are meaningful and must be preserved.
 */

#include "umicom/ui/workbench_state.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void umi_ui_workbench_state_init(UmiUiWorkbenchState *state)
{
    if (state == NULL) return;
    (void)memset(state, 0, sizeof(*state));
    state->sidebar_visible = 1;
    state->auxiliary_sidebar_visible = 1;
    state->bottom_panel_visible = 1;
    state->sidebar_size = 300;
    state->auxiliary_sidebar_size = 340;
    state->bottom_panel_size = 240;
    state->revision = 1U;
}

UmiStatus umi_ui_workbench_state_encode(
    const UmiUiWorkbenchState *state,
    char *out_text,
    size_t capacity)
{
    int written;
    if (state == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(
        out_text, capacity,
        "v1|%s|%s|%s|%s|%d|%d|%d|%" PRId32 "|%" PRId32 "|%" PRId32 "|%" PRIu64,
        state->active_activity,
        state->active_view_container,
        state->active_perspective,
        state->active_document,
        state->sidebar_visible,
        state->auxiliary_sidebar_visible,
        state->bottom_panel_visible,
        state->sidebar_size,
        state->auxiliary_sidebar_size,
        state->bottom_panel_size,
        state->revision);
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    return (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

static UmiStatus next_field(const char **cursor,
                            char *out,
                            size_t capacity,
                            int allow_end)
{
    const char *begin;
    const char *separator;
    size_t length;

    if (cursor == NULL || *cursor == NULL || out == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    begin = *cursor;
    separator = strchr(begin, '|');
    if (separator == NULL) {
        if (!allow_end) return UMI_STATUS_PARSE_ERROR;
        separator = begin + strlen(begin);
    }
    length = (size_t)(separator - begin);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length > 0U) (void)memcpy(out, begin, length);
    out[length] = '\0';
    *cursor = *separator == '|' ? separator + 1 : separator;
    return UMI_STATUS_OK;
}

static int parse_int32_field(const char *text, int32_t *out_value)
{
    char *end = NULL;
    long value;
    if (text == NULL || out_value == NULL || text[0] == '\0') return 0;
    value = strtol(text, &end, 10);
    if (end == NULL || *end != '\0' || value < INT32_MIN || value > INT32_MAX) {
        return 0;
    }
    *out_value = (int32_t)value;
    return 1;
}

static int parse_int_field(const char *text, int *out_value)
{
    int32_t value;
    if (!parse_int32_field(text, &value)) return 0;
    *out_value = value != 0;
    return 1;
}

static int parse_uint64_field(const char *text, uint64_t *out_value)
{
    char *end = NULL;
    unsigned long long value;
    if (text == NULL || out_value == NULL || text[0] == '\0') return 0;
    value = strtoull(text, &end, 10);
    if (end == NULL || *end != '\0') return 0;
    *out_value = (uint64_t)value;
    return 1;
}

UmiStatus umi_ui_workbench_state_decode(
    const char *text,
    UmiUiWorkbenchState *out_state)
{
    const char *cursor;
    char version[8];
    char sidebar_visible[16];
    char auxiliary_visible[16];
    char bottom_visible[16];
    char sidebar_size[32];
    char auxiliary_size[32];
    char bottom_size[32];
    char revision[32];
    UmiStatus status;

    if (text == NULL || out_state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ui_workbench_state_init(out_state);
    cursor = text;

#define READ_FIELD(buffer, allow_end_value) \
    do { \
        status = next_field(&cursor, buffer, sizeof(buffer), allow_end_value); \
        if (status != UMI_STATUS_OK) return status; \
    } while (0)

    READ_FIELD(version, 0);
    READ_FIELD(out_state->active_activity, 0);
    READ_FIELD(out_state->active_view_container, 0);
    READ_FIELD(out_state->active_perspective, 0);
    READ_FIELD(out_state->active_document, 0);
    READ_FIELD(sidebar_visible, 0);
    READ_FIELD(auxiliary_visible, 0);
    READ_FIELD(bottom_visible, 0);
    READ_FIELD(sidebar_size, 0);
    READ_FIELD(auxiliary_size, 0);
    READ_FIELD(bottom_size, 0);
    READ_FIELD(revision, 1);

#undef READ_FIELD

    if (strcmp(version, "v1") != 0 || cursor[0] != '\0' ||
        !parse_int_field(sidebar_visible, &out_state->sidebar_visible) ||
        !parse_int_field(auxiliary_visible,
                         &out_state->auxiliary_sidebar_visible) ||
        !parse_int_field(bottom_visible, &out_state->bottom_panel_visible) ||
        !parse_int32_field(sidebar_size, &out_state->sidebar_size) ||
        !parse_int32_field(auxiliary_size,
                           &out_state->auxiliary_sidebar_size) ||
        !parse_int32_field(bottom_size, &out_state->bottom_panel_size) ||
        !parse_uint64_field(revision, &out_state->revision)) {
        return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}
