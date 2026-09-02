/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workspace_profile_persistence.c
 *
 * PURPOSE:
 *   Encode and decode complete custom workspace profiles without choosing a
 *   database, settings file or frontend toolkit. Products remain responsible
 *   for deciding where the resulting single-line value is stored.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Text fields use percent escaping. This keeps the record safe for the
 * Framework Session Store while still allowing spaces, punctuation and UTF-8
 * profile names. The decoder validates every numeric and identifier field.
 */

#include "umicom/ui/workspace_profile_persistence.h"

#include <ctype.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Provide the append text operation used by this module and its client applications. */
static UmiStatus append_text(char *output,
                             size_t capacity,
                             size_t *length,
                             const char *text)
{
    size_t text_length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL || length == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    text_length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*length + text_length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(output + *length, text, text_length);
    *length += text_length;
    output[*length] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the append number operation used by this module and its client applications. */
static UmiStatus append_number(char *output,
                               size_t capacity,
                               size_t *length,
                               int64_t value)
{
    char number[48];
    int written = snprintf(number, sizeof(number), "|%" PRId64, value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(number)) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    return append_text(output, capacity, length, number);
}

/*
 * Provide the field byte is plain operation used by this module and its client
 * applications.
 */
static int field_byte_is_plain(unsigned char value)
{
    return isalnum(value) || value == ' ' || value == '-' || value == '_' ||
           value == '.';
}

/*
 * Provide the append escaped field operation used by this module and its client
 * applications.
 */
static UmiStatus append_escaped_field(char *output,
                                      size_t capacity,
                                      size_t *length,
                                      const char *text)
{
    static const char HEX[] = "0123456789ABCDEF";
    size_t index;
    UmiStatus status = append_text(output, capacity, length, "|");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; text[index] != '\0'; ++index) {
        unsigned char value = (unsigned char)text[index];
        char encoded[4];
        /* Apply this branch only when its contract condition is satisfied. */
        if (field_byte_is_plain(value)) {
            encoded[0] = (char)value;
            encoded[1] = '\0';
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            encoded[0] = '%';
            encoded[1] = HEX[value >> 4U];
            encoded[2] = HEX[value & 0x0FU];
            encoded[3] = '\0';
        }
        status = append_text(output, capacity, length, encoded);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Write ui workspace profile in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_ui_workspace_profile_encode(
    const UmiUiWorkspaceProfileSnapshot *profile,
    char *out_text,
    size_t capacity)
{
    size_t index;
    size_t length = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || out_text == NULL || capacity == 0U ||
        !umi_ui_id_is_valid(profile->profile_id) ||
        profile->label[0] == '\0' ||
        profile->sidebar_size < 0 ||
        profile->auxiliary_sidebar_size < 0 ||
        profile->bottom_panel_size < 0 ||
        profile->editor_split_mode < UMI_UI_EDITOR_SPLIT_SINGLE ||
        profile->editor_split_mode > UMI_UI_EDITOR_SPLIT_ROWS ||
        (profile->editor_split_ratio != 0 &&
         (profile->editor_split_ratio < UMI_UI_EDITOR_SPLIT_RATIO_MIN ||
          profile->editor_split_ratio > UMI_UI_EDITOR_SPLIT_RATIO_MAX)) ||
        profile->pane_count > UMI_UI_WORKSPACE_PROFILE_MAX_PANES) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile->pane_count; ++index) {
        const UmiUiWorkspacePanePlacement *pane = &profile->panes[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_ui_id_is_valid(pane->pane_id) ||
            (pane->placement != UMI_UI_PLACEMENT_LEFT &&
             pane->placement != UMI_UI_PLACEMENT_RIGHT &&
             pane->placement != UMI_UI_PLACEMENT_TOP &&
             pane->placement != UMI_UI_PLACEMENT_BOTTOM &&
             pane->placement != UMI_UI_PLACEMENT_CENTRE)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    out_text[0] = '\0';
    status = append_text(out_text, capacity, &length, "v2");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_escaped_field(
        out_text, capacity, &length, profile->profile_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_escaped_field(
        out_text, capacity, &length, profile->label);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_escaped_field(
        out_text, capacity, &length, profile->description);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_escaped_field(
        out_text, capacity, &length, profile->icon_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_number(
        out_text, capacity, &length, profile->sidebar_visible != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_number(
        out_text, capacity, &length, profile->auxiliary_sidebar_visible != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_number(
        out_text, capacity, &length, profile->bottom_panel_visible != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_number(
        out_text, capacity, &length, profile->sidebar_size);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_number(
        out_text, capacity, &length, profile->auxiliary_sidebar_size);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_number(
        out_text, capacity, &length, profile->bottom_panel_size);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_number(
        out_text, capacity, &length, profile->order);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_number(
        out_text, capacity, &length, profile->active != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_number(
        out_text, capacity, &length, profile->built_in != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_number(
        out_text, capacity, &length, profile->locked != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_number(
        out_text, capacity, &length, profile->editor_split_mode);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_number(
        out_text, capacity, &length,
        profile->editor_split_ratio == 0
            ? UMI_UI_EDITOR_SPLIT_RATIO_DEFAULT
            : profile->editor_split_ratio);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = append_number(
        out_text, capacity, &length, (int64_t)profile->pane_count);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK &&
         index < profile->pane_count; ++index) {
        const UmiUiWorkspacePanePlacement *pane = &profile->panes[index];
        status = append_escaped_field(out_text, capacity, &length,
                                      pane->pane_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = append_number(
            out_text, capacity, &length, pane->placement);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = append_number(
            out_text, capacity, &length, pane->order);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = append_number(
            out_text, capacity, &length, pane->visible != 0);
    }
    return status;
}

/* Provide the next field operation used by this module and its client applications. */
static UmiStatus next_field(const char **cursor,
                            char *out_field,
                            size_t capacity,
                            int allow_end)
{
    const char *separator;
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor == NULL || *cursor == NULL || out_field == NULL ||
        capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    separator = strchr(*cursor, '|');
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (separator == NULL) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (!allow_end) return UMI_STATUS_PARSE_ERROR;
        separator = *cursor + strlen(*cursor);
    }
    length = (size_t)(separator - *cursor);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out_field, *cursor, length);
    out_field[length] = '\0';
    *cursor = *separator == '|' ? separator + 1 : separator;
    return UMI_STATUS_OK;
}

/* Provide the hex value operation used by this module and its client applications. */
static int hex_value(char value)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= '0' && value <= '9') return value - '0';
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= 'A' && value <= 'F') return value - 'A' + 10;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    return -1;
}

/* Provide the decode escaped operation used by this module and its client applications. */
static UmiStatus decode_escaped(const char *input,
                                char *output,
                                size_t capacity)
{
    size_t read_index;
    size_t write_index = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL || output == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (read_index = 0U; input[read_index] != '\0'; ++read_index) {
        unsigned char value = (unsigned char)input[read_index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (value == '%') {
            int high;
            int low;
            /* Apply this branch only when its contract condition is satisfied. */
            if (input[read_index + 1U] == '\0' ||
                input[read_index + 2U] == '\0') {
                return UMI_STATUS_PARSE_ERROR;
            }
            high = hex_value(input[read_index + 1U]);
            low = hex_value(input[read_index + 2U]);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (high < 0 || low < 0) return UMI_STATUS_PARSE_ERROR;
            value = (unsigned char)((high << 4U) | low);
            read_index += 2U;
        }
        /* Embedded NUL bytes would truncate the decoded C string and could
         * make later identifier validation inspect a different value from the
         * one carried by the record. Reject them at the codec boundary. */
        if (value == 0U) return UMI_STATUS_PARSE_ERROR;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (write_index + 1U >= capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        output[write_index++] = (char)value;
    }
    output[write_index] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the parse integer operation used by this module and its client applications. */
static int parse_integer(const char *text, int64_t *out_value)
{
    char *end = NULL;
    long long value;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_value == NULL || text[0] == '\0') return 0;
    value = strtoll(text, &end, 10);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (end == NULL || *end != '\0') return 0;
    *out_value = (int64_t)value;
    return 1;
}

/*
 * Read ui workspace profile into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_ui_workspace_profile_decode(
    const char *text,
    UmiUiWorkspaceProfileSnapshot *out_profile)
{
    const char *cursor;
    char field[UMI_UI_DESCRIPTION_CAPACITY * 3U];
    int64_t values[13];
    size_t value_count;
    size_t value_index;
    size_t pane_index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_profile, 0, sizeof(*out_profile));
    out_profile->editor_split_mode = UMI_UI_EDITOR_SPLIT_SINGLE;
    out_profile->editor_split_ratio = UMI_UI_EDITOR_SPLIT_RATIO_DEFAULT;
    cursor = text;
    status = next_field(&cursor, field, sizeof(field), 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK ||
        (strcmp(field, "v1") != 0 && strcmp(field, "v2") != 0)) {
        return UMI_STATUS_PARSE_ERROR;
    }
    value_count = strcmp(field, "v2") == 0 ? 13U : 11U;

#define READ_ESCAPED(destination) \
    do { \
        status = next_field(&cursor, field, sizeof(field), 0); \
        if (status != UMI_STATUS_OK) return status; \
        status = decode_escaped(field, destination, sizeof(destination)); \
        if (status != UMI_STATUS_OK) return status; \
    } while (0)

    READ_ESCAPED(out_profile->profile_id);
    READ_ESCAPED(out_profile->label);
    READ_ESCAPED(out_profile->description);
    READ_ESCAPED(out_profile->icon_name);

#undef READ_ESCAPED

    /* Visit each bounded item once so every record receives the same rule. */
    for (value_index = 0U; value_index < value_count; ++value_index) {
        status = next_field(&cursor, field, sizeof(field),
                            value_index + 1U == value_count);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK ||
            !parse_integer(field, &values[value_index])) {
            return UMI_STATUS_PARSE_ERROR;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (values[3] < 0 || values[3] > INT32_MAX ||
        values[4] < 0 || values[4] > INT32_MAX ||
        values[5] < 0 || values[5] > INT32_MAX ||
        values[6] < INT32_MIN || values[6] > INT32_MAX ||
        values[value_count - 1U] < 0 ||
        values[value_count - 1U] >
            (int64_t)UMI_UI_WORKSPACE_PROFILE_MAX_PANES) {
        return UMI_STATUS_PARSE_ERROR;
    }
    out_profile->sidebar_visible = values[0] != 0;
    out_profile->auxiliary_sidebar_visible = values[1] != 0;
    out_profile->bottom_panel_visible = values[2] != 0;
    out_profile->sidebar_size = (int32_t)values[3];
    out_profile->auxiliary_sidebar_size = (int32_t)values[4];
    out_profile->bottom_panel_size = (int32_t)values[5];
    out_profile->order = (int32_t)values[6];
    out_profile->active = values[7] != 0;
    out_profile->built_in = values[8] != 0;
    out_profile->locked = values[9] != 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (value_count == 13U) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (values[10] < UMI_UI_EDITOR_SPLIT_SINGLE ||
            values[10] > UMI_UI_EDITOR_SPLIT_ROWS ||
            values[11] < UMI_UI_EDITOR_SPLIT_RATIO_MIN ||
            values[11] > UMI_UI_EDITOR_SPLIT_RATIO_MAX) {
            return UMI_STATUS_PARSE_ERROR;
        }
        out_profile->editor_split_mode =
            (UmiUiEditorSplitMode)values[10];
        out_profile->editor_split_ratio = (int32_t)values[11];
    }
    out_profile->pane_count = (size_t)values[value_count - 1U];

    /* Visit each bounded item once so every record receives the same rule. */
    for (pane_index = 0U; pane_index < out_profile->pane_count; ++pane_index) {
        int64_t placement;
        int64_t order;
        int64_t visible;
        status = next_field(&cursor, field, sizeof(field), 0);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = decode_escaped(
            field,
            out_profile->panes[pane_index].pane_id,
            sizeof(out_profile->panes[pane_index].pane_id));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = next_field(&cursor, field, sizeof(field), 0);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK || !parse_integer(field, &placement)) {
            return UMI_STATUS_PARSE_ERROR;
        }
        status = next_field(&cursor, field, sizeof(field), 0);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK || !parse_integer(field, &order)) {
            return UMI_STATUS_PARSE_ERROR;
        }
        status = next_field(&cursor, field, sizeof(field),
                            pane_index + 1U == out_profile->pane_count);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK || !parse_integer(field, &visible)) {
            return UMI_STATUS_PARSE_ERROR;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((placement != UMI_UI_PLACEMENT_LEFT &&
             placement != UMI_UI_PLACEMENT_RIGHT &&
             placement != UMI_UI_PLACEMENT_TOP &&
             placement != UMI_UI_PLACEMENT_BOTTOM &&
             placement != UMI_UI_PLACEMENT_CENTRE) ||
            order < INT32_MIN || order > INT32_MAX) {
            return UMI_STATUS_PARSE_ERROR;
        }
        out_profile->panes[pane_index].placement = (UmiUiPlacement)placement;
        out_profile->panes[pane_index].order = (int32_t)order;
        out_profile->panes[pane_index].visible = visible != 0;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_ui_id_is_valid(out_profile->panes[pane_index].pane_id)) {
            return UMI_STATUS_PARSE_ERROR;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (cursor[0] != '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_ui_id_is_valid(out_profile->profile_id) ||
        out_profile->label[0] == '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}
