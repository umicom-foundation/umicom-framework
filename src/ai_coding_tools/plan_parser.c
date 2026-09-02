/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/plan_parser.c
 *
 * PURPOSE:
 *   Implement strict textual multi-step tool-plan parsing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/plan_parser.h"

#include <string.h>

#define PLAN_HEADER "UMICOM-TOOL-PLAN/1"

typedef enum ParseMode {
    MODE_NORMAL = 0,
    MODE_RATIONALE = 1,
    MODE_ARGUMENTS = 2
} ParseMode;

/* Provide the line equals operation used by this module and its client applications. */
static int line_equals(const char *line, size_t length, const char *value)
{
    const size_t value_length = strlen(value);
    return length == value_length &&
        strncmp(line, value, length) == 0;
}

/* Provide the append line operation used by this module and its client applications. */
static UmiStatus append_line(
    char *buffer,
    size_t capacity,
    size_t *used,
    const char *line,
    size_t length)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*used + length + 2U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) {
        (void)memcpy(buffer + *used, line, length);
        *used += length;
    }

    buffer[(*used)++] = '\n';
    buffer[*used] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the parse plan line operation used by this module and its client applications. */
static UmiStatus parse_plan_line(
    const char *line,
    size_t length,
    char *out_id,
    size_t id_capacity,
    char *out_title,
    size_t title_capacity)
{
    const char *id;
    const char *separator;
    size_t id_length;
    size_t title_length;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length <= 5U || strncmp(line, "PLAN|", 5U) != 0) {
        return UMI_STATUS_PARSE_ERROR;
    }

    id = line + 5U;
    separator = memchr(id, '|', length - 5U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (separator == NULL) return UMI_STATUS_PARSE_ERROR;

    id_length = (size_t)(separator - id);
    title_length = length - 5U - id_length - 1U;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (id_length == 0U || id_length >= id_capacity ||
        title_length == 0U || title_length >= title_capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(out_id, id, id_length);
    out_id[id_length] = '\0';
    (void)memcpy(out_title, separator + 1, title_length);
    out_title[title_length] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the parse step line operation used by this module and its client applications. */
static UmiStatus parse_step_line(
    const char *line,
    size_t length,
    UmiAiCodingToolPlanStep *step)
{
    char buffer[1024];
    char *parts[4];
    char *cursor;
    size_t part_count = 0U;
    size_t copy_length;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length <= 11U || strncmp(line, "STEP-BEGIN|", 11U) != 0) {
        return UMI_STATUS_PARSE_ERROR;
    }

    copy_length = length - 11U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (copy_length >= sizeof(buffer)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(buffer, line + 11U, copy_length);
    buffer[copy_length] = '\0';

    cursor = buffer;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (part_count < 4U) {
        char *separator = strchr(cursor, '|');

        parts[part_count++] = cursor;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (separator == NULL) break;
        *separator = '\0';
        cursor = separator + 1;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (part_count != 4U ||
        parts[0][0] == '\0' ||
        parts[1][0] == '\0' ||
        strlen(parts[0]) >= sizeof(step->step_id) ||
        strlen(parts[1]) >= sizeof(step->call.tool_id) ||
        (strcmp(parts[2], "0") != 0 && strcmp(parts[2], "1") != 0) ||
        (strcmp(parts[3], "0") != 0 && strcmp(parts[3], "1") != 0)) {
        return UMI_STATUS_PARSE_ERROR;
    }

    (void)strcpy(step->step_id, parts[0]);
    (void)strcpy(step->call.tool_id, parts[1]);
    step->required = strcmp(parts[2], "1") == 0;
    step->continue_on_failure = strcmp(parts[3], "1") == 0;
    step->call.state = UMI_AI_CODING_TOOL_CALL_PENDING;
    step->call.status = UMI_STATUS_OK;
    return UMI_STATUS_OK;
}

/*
 * Read ai coding tool plan into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_ai_coding_tool_plan_parse(
    const char *text,
    uint64_t first_call_id,
    UmiAiCodingToolPlan *out_plan)
{
    const char *cursor;
    ParseMode mode = MODE_NORMAL;
    UmiAiCodingToolPlanStep step;
    char plan_id[UMI_AI_CODING_TOOL_ID_CAPACITY];
    char title[256];
    char rationale[2048];
    size_t rationale_length = 0U;
    size_t argument_length = 0U;
    size_t parsed_steps = 0U;
    int header_seen = 0;
    int plan_line_seen = 0;
    int plan_end_seen = 0;
    int step_active = 0;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_plan == NULL || first_call_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&step, 0, sizeof(step));
    plan_id[0] = '\0';
    title[0] = '\0';
    rationale[0] = '\0';

    cursor = text;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        size_t length =
            end != NULL ? (size_t)(end - cursor) : strlen(cursor);

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length > 0U && cursor[length - 1U] == '\r') --length;

        /* Apply this branch only when its contract condition is satisfied. */
        if (!header_seen) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (!line_equals(cursor, length, PLAN_HEADER)) {
                return UMI_STATUS_PARSE_ERROR;
            }
            header_seen = 1;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (mode == MODE_RATIONALE) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (line_equals(cursor, length, "RATIONALE-END")) {
                mode = MODE_NORMAL;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                status = append_line(
                    rationale,
                    sizeof(rationale),
                    &rationale_length,
                    cursor,
                    length);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) return status;
            }
        } else /* Apply this branch only when its contract condition is satisfied. */ if (mode == MODE_ARGUMENTS) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (line_equals(cursor, length, "ARGUMENTS-END")) {
                mode = MODE_NORMAL;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                status = append_line(
                    step.call.arguments_json,
                    sizeof(step.call.arguments_json),
                    &argument_length,
                    cursor,
                    length);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) return status;
            }
        } else /* Apply this branch only when its contract condition is satisfied. */ if (!plan_line_seen &&
                   length > 5U &&
                   strncmp(cursor, "PLAN|", 5U) == 0) {
            status = parse_plan_line(
                cursor,
                length,
                plan_id,
                sizeof(plan_id),
                title,
                sizeof(title));
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
            plan_line_seen = 1;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (line_equals(cursor, length, "RATIONALE-BEGIN")) {
            mode = MODE_RATIONALE;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (length > 11U &&
                   strncmp(cursor, "STEP-BEGIN|", 11U) == 0) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (step_active) return UMI_STATUS_PARSE_ERROR;

            (void)memset(&step, 0, sizeof(step));
            argument_length = 0U;

            status = parse_step_line(cursor, length, &step);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;

            step.call.call_id = first_call_id + parsed_steps;
            step.call.sequence = parsed_steps + 1U;
            step.call.approved = 0;
            step.has_dependency = parsed_steps > 0U;
            step.depends_on_index =
                parsed_steps > 0U ? parsed_steps - 1U : 0U;
            step_active = 1;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (line_equals(cursor, length, "ARGUMENTS-BEGIN")) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (!step_active) return UMI_STATUS_PARSE_ERROR;
            mode = MODE_ARGUMENTS;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (line_equals(cursor, length, "STEP-END")) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (!step_active) return UMI_STATUS_PARSE_ERROR;

            /* Apply this branch only when its contract condition is satisfied. */
            if (argument_length > 0U &&
                step.call.arguments_json[argument_length - 1U] == '\n') {
                step.call.arguments_json[argument_length - 1U] = '\0';
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (step.call.arguments_json[0] == '\0') {
                (void)strcpy(step.call.arguments_json, "{}");
            }

            /* Apply this branch only when its contract condition is satisfied. */
            if (parsed_steps == 0U) {
                status = umi_ai_coding_tool_plan_init(
                    out_plan,
                    plan_id,
                    title,
                    rationale);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) return status;
            }

            status = umi_ai_coding_tool_plan_add(out_plan, &step);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;

            parsed_steps += 1U;
            step_active = 0;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (line_equals(cursor, length, "PLAN-END")) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (step_active) return UMI_STATUS_PARSE_ERROR;
            plan_end_seen = 1;
        }

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL) break;
        cursor = end + 1;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!header_seen || !plan_line_seen || !plan_end_seen ||
        mode != MODE_NORMAL || step_active || parsed_steps == 0U) {
        return UMI_STATUS_PARSE_ERROR;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (rationale_length > 0U &&
        rationale[rationale_length - 1U] == '\n') {
        rationale[rationale_length - 1U] = '\0';
        (void)strcpy(out_plan->rationale, rationale);
    }

    return UMI_STATUS_OK;
}
