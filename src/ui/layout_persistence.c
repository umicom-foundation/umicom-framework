/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/layout_persistence.c
 *
 * PURPOSE:
 *   Save and restore portable workspace layouts while remaining compatible
 *   with layouts written before placement, tab-stack and context-link state
 *   became separate concepts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/layout_persistence.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

/* Provide the safe field operation used by this module and its client applications. */
static bool safe_field(const char *text)
{
    return text != NULL && strchr(text, '\t') == NULL &&
        strchr(text, '\n') == NULL && strchr(text, '\r') == NULL;
}

/* Provide the field or dash operation used by this module and its client applications. */
static const char *field_or_dash(const char *text)
{
    return text != NULL && text[0] != '\0' ? text : "-";
}

/*
 * Provide the restore optional field operation used by this module and its client
 * applications.
 */
static void restore_optional_field(char *destination,
                                   size_t capacity,
                                   const char *field)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    destination[0] = '\0';
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (field != NULL && strcmp(field, "-") != 0)
        (void)snprintf(destination, capacity, "%s", field);
}

/* Provide the append text operation used by this module and its client applications. */
static UmiStatus append_text(char *out_text,
                             size_t capacity,
                             size_t *used,
                             const char *text)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_text == NULL || used == NULL || text == NULL || *used > capacity)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(text);
    /* Subtract first so a hostile length cannot wrap the addition and pass
     * the capacity check on platforms where size_t is unsigned. */
    if (length >= capacity - *used) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out_text + *used, text, length + 1U);
    *used += length;
    return UMI_STATUS_OK;
}

/* Provide the append window v2 operation used by this module and its client applications. */
static UmiStatus append_window_v2(const UmiUiWorkspaceWindow *window,
                                  char *out_text,
                                  size_t capacity,
                                  size_t *used)
{
    char line[1024U];
    int length;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!safe_field(window->window_id) || !safe_field(window->title) ||
        !safe_field(window->tool_id) || !safe_field(window->group_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    length = snprintf(
        line, sizeof(line),
        "W\t%s\t%s\t%s\t%s\t%.8f\t%.8f\t%.8f\t%.8f\t%d\t%d\t%d\t%d\t%d\n",
        window->window_id, window->title, window->tool_id,
        field_or_dash(window->group_id), window->x, window->y,
        window->width, window->height, window->visible ? 1 : 0,
        window->floating ? 1 : 0, window->maximised ? 1 : 0,
        window->closable ? 1 : 0, window->z_order);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length < 0 || (size_t)length >= sizeof(line))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return append_text(out_text, capacity, used, line);
}

/* Provide the append window v3 operation used by this module and its client applications. */
static UmiStatus append_window_v3(const UmiUiWorkspaceWindow *window,
                                  char *out_text,
                                  size_t capacity,
                                  size_t *used)
{
    char line[1536U];
    int length;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!safe_field(window->window_id) || !safe_field(window->title) ||
        !safe_field(window->tool_id) || !safe_field(window->group_id) ||
        !safe_field(window->placement_id) || !safe_field(window->stack_id) ||
        !safe_field(window->context_group_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    length = snprintf(
        line, sizeof(line),
        /* Seventeen significant digits preserve every finite double when
         * a saved panel touches the normalized right or bottom boundary. */
        "W\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%.17g\t%.17g\t%.17g\t%.17g"
        "\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
        window->window_id, window->title, window->tool_id,
        field_or_dash(window->group_id),
        field_or_dash(window->placement_id),
        field_or_dash(window->stack_id),
        field_or_dash(window->context_group_id),
        window->x, window->y, window->width, window->height,
        window->visible ? 1 : 0, window->floating ? 1 : 0,
        window->maximised ? 1 : 0, window->closable ? 1 : 0,
        window->pinned ? 1 : 0, window->resizable ? 1 : 0,
        window->z_order);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length < 0 || (size_t)length >= sizeof(line))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return append_text(out_text, capacity, used, line);
}

/*
 * Write ui layout persistence in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_ui_layout_persistence_encode(
    const UmiUiLayoutPersistenceRecord *record,
    char *out_text,
    size_t capacity)
{
    char line[1024U];
    const char *magic;
    size_t used = 0U;
    size_t index;
    int length;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || out_text == NULL || capacity == 0U ||
        (record->schema_version != 2U &&
         record->schema_version != UMI_UI_LAYOUT_PERSISTENCE_SCHEMA_VERSION) ||
        !safe_field(record->layout.layout_id) ||
        record->layout.layout_id[0] == '\0' ||
        !safe_field(record->layout.name) || record->layout.name[0] == '\0' ||
        record->layout.window_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS)
        return UMI_STATUS_INVALID_ARGUMENT;
    magic = record->schema_version == 2U ? "UMILAYOUT2" : "UMILAYOUT3";
    out_text[0] = '\0';
    length = snprintf(
        line, sizeof(line), "%s\t%u\t%llu\t%s\t%s\t%d\t%zu\t%llu\n",
        magic, record->schema_version,
        (unsigned long long)record->saved_at_ns, record->layout.layout_id,
        record->layout.name, record->layout.locked ? 1 : 0,
        record->layout.window_count,
        (unsigned long long)record->layout.revision);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length < 0 || (size_t)length >= sizeof(line))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = append_text(out_text, capacity, &used, line);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < record->layout.window_count && status == UMI_STATUS_OK;
         ++index) {
        status = record->schema_version == 2U
            ? append_window_v2(&record->layout.windows[index], out_text,
                               capacity, &used)
            : append_window_v3(&record->layout.windows[index], out_text,
                               capacity, &used);
    }
    return status;
}

/* Split literal tabs, not scanf whitespace, and require the exact schema
 * field count. Empty fields are not emitted by either supported encoder. */
static bool split_fields(char *line, char **fields, size_t count)
{
    size_t index;
    for (index = 0U; index < count; ++index) {
        char *separator;
        fields[index] = line;
        separator = strchr(line, '\t');
        if (index + 1U == count) {
            if (separator != NULL) return false;
        } else {
            if (separator == NULL) return false;
            *separator = '\0';
        }
        if (line[0] == '\0' || strchr(line, '\r') != NULL) return false;
        if (separator != NULL) line = separator + 1;
    }
    return true;
}

/* Validate text capacity before copying required and dash-encoded optional
 * fields. Historical fixed-decimal records keep the same field meanings. */
static bool decode_text(char *destination, size_t capacity, const char *field, bool optional)
{
    const size_t length = strlen(field);
    if (length == 0U || length >= capacity) return false;
    if (optional) restore_optional_field(destination, capacity, field);
    else memcpy(destination, field, length + 1U);
    return true;
}

/* Unsigned metadata accepts digits only and rejects overflow before a cast;
 * negative revisions and signed counts are not valid checkpoint identities. */
static bool decode_u64(const char *field, uint64_t *value)
{
    const unsigned char *scan = (const unsigned char *)field;
    unsigned long long parsed;
    char *end;
    if (*scan == 0U) return false;
    for (; *scan != 0U; ++scan) if (*scan < '0' || *scan > '9') return false;
    errno = 0;
    parsed = strtoull(field, &end, 10);
    if (errno == ERANGE || *end != '\0') return false;
#if ULLONG_MAX > UINT64_MAX
    /* Unusual platforms may have an unsigned long long wider than uint64_t. */
    if (parsed > UINT64_MAX) return false;
#endif
    *value = (uint64_t)parsed;
    return true;
}

/* Boolean flags have exactly two representations, avoiding silent policy
 * changes when a malformed producer writes arbitrary nonzero values. */
static bool decode_bool(const char *field, bool *value)
{
    if (strcmp(field, "0") == 0) { *value = false; return true; }
    if (strcmp(field, "1") == 0) { *value = true; return true; }
    return false;
}

/* Keep negative z-order valid, but reject whitespace, trailing suffixes and
 * values which cannot be represented by the public signed 32-bit field. */
static bool decode_z_order(const char *field, int32_t *value)
{
    const unsigned char *scan = (const unsigned char *)field;
    long long parsed;
    char *end;
    if (*scan == '-') ++scan;
    if (*scan == 0U) return false;
    for (; *scan != 0U; ++scan) if (*scan < '0' || *scan > '9') return false;
    errno = 0;
    parsed = strtoll(field, &end, 10);
    if (errno == ERANGE || *end != '\0' || parsed < INT32_MIN || parsed > INT32_MAX) return false;
    *value = (int32_t)parsed;
    return true;
}

/* Accept old fixed decimals and round-trip exponent notation equally. NaN,
 * infinity, hexadecimal and overflow never become native window geometry. */
static bool decode_coordinate(const char *field, double *value)
{
    const unsigned char *scan = (const unsigned char *)field;
    char *end;
    double parsed;
    if (*scan == 0U) return false;
    for (; *scan != 0U; ++scan)
        if (!((*scan >= '0' && *scan <= '9') || *scan == '.' || *scan == '-' ||
              *scan == '+' || *scan == 'e' || *scan == 'E')) return false;
    errno = 0;
    parsed = strtod(field, &end);
    if (errno == ERANGE || end == field || *end != '\0' || !isfinite(parsed)) return false;
    *value = parsed;
    return true;
}

/* Decode the older schema without interpreting its single group as a modern
 * independent placement or linked-context field. */
static UmiStatus decode_window_v2(char *line,
                                  UmiUiWorkspaceWindow *window)
{
    char *fields[14];
    /* Reject the complete row on any failed conversion; no truncated field
     * or partially consumed number becomes an apparently valid panel. */
    if (!split_fields(line, fields, 14U) || strcmp(fields[0], "W") != 0 ||
        !decode_text(window->window_id, sizeof(window->window_id), fields[1], false) ||
        !decode_text(window->title, sizeof(window->title), fields[2], false) ||
        !decode_text(window->tool_id, sizeof(window->tool_id), fields[3], false) ||
        !decode_text(window->group_id, sizeof(window->group_id), fields[4], true) ||
        !decode_text(window->stack_id, sizeof(window->stack_id), fields[4], true) ||
        !decode_coordinate(fields[5], &window->x) || !decode_coordinate(fields[6], &window->y) ||
        !decode_coordinate(fields[7], &window->width) || !decode_coordinate(fields[8], &window->height) ||
        !decode_bool(fields[9], &window->visible) || !decode_bool(fields[10], &window->floating) ||
        !decode_bool(fields[11], &window->maximised) || !decode_bool(fields[12], &window->closable) ||
        !decode_z_order(fields[13], &window->z_order)) return UMI_STATUS_PARSE_ERROR;
    window->resizable = true;
    return UMI_STATUS_OK;
}

/* Decode all modern placement, context and policy fields without requiring
 * the old and new encoder's decimal formatting to be byte-for-byte equal. */
static UmiStatus decode_window_v3(char *line,
                                  UmiUiWorkspaceWindow *window)
{
    char *fields[19];
    /* Each index follows the existing schema3 field order; the parser only
     * tightens validation and adds no second layout representation. */
    if (!split_fields(line, fields, 19U) || strcmp(fields[0], "W") != 0 ||
        !decode_text(window->window_id, sizeof(window->window_id), fields[1], false) ||
        !decode_text(window->title, sizeof(window->title), fields[2], false) ||
        !decode_text(window->tool_id, sizeof(window->tool_id), fields[3], false) ||
        !decode_text(window->group_id, sizeof(window->group_id), fields[4], true) ||
        !decode_text(window->placement_id, sizeof(window->placement_id), fields[5], true) ||
        !decode_text(window->stack_id, sizeof(window->stack_id), fields[6], true) ||
        !decode_text(window->context_group_id, sizeof(window->context_group_id), fields[7], true) ||
        !decode_coordinate(fields[8], &window->x) || !decode_coordinate(fields[9], &window->y) ||
        !decode_coordinate(fields[10], &window->width) || !decode_coordinate(fields[11], &window->height) ||
        !decode_bool(fields[12], &window->visible) || !decode_bool(fields[13], &window->floating) ||
        !decode_bool(fields[14], &window->maximised) || !decode_bool(fields[15], &window->closable) ||
        !decode_bool(fields[16], &window->pinned) || !decode_bool(fields[17], &window->resizable) ||
        !decode_z_order(fields[18], &window->z_order)) return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}

/*
 * Read ui layout persistence into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_ui_layout_persistence_decode(
    const char *text,
    UmiUiLayoutPersistenceRecord *out_record)
{
    char buffer[UMI_UI_LAYOUT_ENCODED_CAPACITY];
    char *line;
    uint64_t expected;
    size_t parsed = 0U;
    uint64_t schema;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_record == NULL || strlen(text) >= sizeof(buffer))
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)snprintf(buffer, sizeof(buffer), "%s", text);
    (void)memset(out_record, 0, sizeof(*out_record));
    line = buffer;
    {
        char *end = strchr(line, '\n');
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL) return UMI_STATUS_PARSE_ERROR;
        *end = '\0';
        /* Windows text tools may translate LF to CRLF. Strip exactly its
         * terminal CR; embedded or repeated carriage returns remain invalid. */
        if (end > line && end[-1] == '\r') end[-1] = '\0';
        /* Header conversions are bounded before assigning narrower public
         * fields. Literal separators and complete numbers prevent ambiguity. */
        char *fields[8];
        if (!split_fields(line, fields, 8U) || !decode_u64(fields[1], &schema) ||
            !decode_u64(fields[2], &out_record->saved_at_ns) ||
            !decode_text(out_record->layout.layout_id, sizeof(out_record->layout.layout_id), fields[3], false) ||
            !decode_text(out_record->layout.name, sizeof(out_record->layout.name), fields[4], false) ||
            !decode_bool(fields[5], &out_record->layout.locked) ||
            !decode_u64(fields[6], &expected) || !decode_u64(fields[7], &out_record->layout.revision) ||
            !((schema == 2U && strcmp(fields[0], "UMILAYOUT2") == 0) ||
              (schema == UMI_UI_LAYOUT_PERSISTENCE_SCHEMA_VERSION &&
               strcmp(fields[0], "UMILAYOUT3") == 0)) ||
            expected > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS)
            return UMI_STATUS_PARSE_ERROR;
        out_record->schema_version = (uint32_t)schema;
        line = end + 1;
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*line != '\0' && parsed < expected) {
        UmiUiWorkspaceWindow *window = &out_record->layout.windows[parsed];
        char *end = strchr(line, '\n');
        bool had_newline = end != NULL;
        UmiStatus status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!had_newline) end = line + strlen(line);
        /* Apply this branch only when its contract condition is satisfied. */
        if (had_newline) *end = '\0';
        /* Preserve the same CRLF compatibility for each complete panel row.
         * A final row without LF remains supported, but a bare CR does not. */
        if (had_newline && end > line && end[-1] == '\r') end[-1] = '\0';
        status = schema == 2U
            ? decode_window_v2(line, window)
            : decode_window_v3(line, window);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* A window identity denotes exactly one instance in either schema. */
        for (size_t previous = 0U; previous < parsed; ++previous)
            if (strcmp(out_record->layout.windows[previous].window_id, window->window_id) == 0)
                return UMI_STATUS_PARSE_ERROR;
        parsed += 1U;
        line = had_newline ? end + 1 : end;
    }
    out_record->layout.window_count = parsed;
    /* The declared count must consume the complete record. Ignoring trailing
     * rows would let a producer and consumer disagree about reviewed content. */
    return parsed == expected && *line == '\0'
        ? UMI_STATUS_OK
        : UMI_STATUS_PARSE_ERROR;
}
