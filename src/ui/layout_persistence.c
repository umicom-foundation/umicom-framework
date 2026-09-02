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
    if (out_text == NULL || used == NULL || text == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*used + length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
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
        "W\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%.8f\t%.8f\t%.8f\t%.8f"
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

/* Provide the decode window v2 operation used by this module and its client applications. */
static UmiStatus decode_window_v2(char *line,
                                  UmiUiWorkspaceWindow *window)
{
    char group[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    int visible;
    int floating;
    int maximised;
    int closable;
    int matched = sscanf(
        line,
        "W\t%127[^\t]\t%191[^\t]\t%127[^\t]\t%127[^\t]"
        "\t%lf\t%lf\t%lf\t%lf\t%d\t%d\t%d\t%d\t%d",
        window->window_id, window->title, window->tool_id, group,
        &window->x, &window->y, &window->width, &window->height,
        &visible, &floating, &maximised, &closable, &window->z_order);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (matched != 13) return UMI_STATUS_PARSE_ERROR;
    restore_optional_field(window->group_id, sizeof(window->group_id), group);
    restore_optional_field(window->stack_id, sizeof(window->stack_id), group);
    window->visible = visible != 0;
    window->floating = floating != 0;
    window->maximised = maximised != 0;
    window->closable = closable != 0;
    window->resizable = true;
    return UMI_STATUS_OK;
}

/* Provide the decode window v3 operation used by this module and its client applications. */
static UmiStatus decode_window_v3(char *line,
                                  UmiUiWorkspaceWindow *window)
{
    char group[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char placement[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char stack[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char context[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    int visible;
    int floating;
    int maximised;
    int closable;
    int pinned;
    int resizable;
    int matched = sscanf(
        line,
        "W\t%127[^\t]\t%191[^\t]\t%127[^\t]\t%127[^\t]"
        "\t%127[^\t]\t%127[^\t]\t%127[^\t]"
        "\t%lf\t%lf\t%lf\t%lf\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
        window->window_id, window->title, window->tool_id, group,
        placement, stack, context, &window->x, &window->y,
        &window->width, &window->height, &visible, &floating,
        &maximised, &closable, &pinned, &resizable, &window->z_order);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (matched != 18) return UMI_STATUS_PARSE_ERROR;
    restore_optional_field(window->group_id, sizeof(window->group_id), group);
    restore_optional_field(window->placement_id,
                           sizeof(window->placement_id), placement);
    restore_optional_field(window->stack_id, sizeof(window->stack_id), stack);
    restore_optional_field(window->context_group_id,
                           sizeof(window->context_group_id), context);
    window->visible = visible != 0;
    window->floating = floating != 0;
    window->maximised = maximised != 0;
    window->closable = closable != 0;
    window->pinned = pinned != 0;
    window->resizable = resizable != 0;
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
    char magic[16U];
    char *line;
    size_t expected;
    size_t parsed = 0U;
    unsigned schema;
    unsigned long long saved;
    unsigned long long revision;
    int locked;
    int matched;
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
        matched = sscanf(
            line, "%15[^\t]\t%u\t%llu\t%127[^\t]\t%191[^\t]"
                  "\t%d\t%zu\t%llu",
            magic, &schema, &saved, out_record->layout.layout_id,
            out_record->layout.name, &locked, &expected, &revision);
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (matched != 8 ||
            !((schema == 2U && strcmp(magic, "UMILAYOUT2") == 0) ||
              (schema == UMI_UI_LAYOUT_PERSISTENCE_SCHEMA_VERSION &&
               strcmp(magic, "UMILAYOUT3") == 0)) ||
            expected > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS)
            return UMI_STATUS_PARSE_ERROR;
        out_record->schema_version = schema;
        out_record->saved_at_ns = (uint64_t)saved;
        out_record->layout.locked = locked != 0;
        out_record->layout.revision = (uint64_t)revision;
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
        status = schema == 2U
            ? decode_window_v2(line, window)
            : decode_window_v3(line, window);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
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
