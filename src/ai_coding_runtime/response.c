/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/response.c
 *
 * PURPOSE:
 *   Parse the strict Umicom coding response protocol into the established
 *   conflict-aware UmiAiCodingPatch complete-file transaction.
 *
 * SECURITY:
 *   Model-supplied paths are normalized and checked against the controlled
 *   workspace adapter. MODIFY and DELETE operations materialize the current
 *   before-text themselves; a model cannot forge the conflict hash.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/response.h"

#include <stdio.h>
#include <string.h>

#define RESPONSE_HEADER "UMICOM-CODING-RESPONSE/1"

typedef enum ParseMode {
    PARSE_NORMAL = 0,
    PARSE_SUMMARY = 1,
    PARSE_RATIONALE = 2,
    PARSE_CONTENT = 3
} ParseMode;

typedef struct ParsedFile {
    UmiAiCodingPatchOperation operation;
    char path[UMI_AI_TEXT_CAPACITY];
    char content[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    size_t content_length;
    int active;
} ParsedFile;

/* Provide the append line operation used by this module and its client applications. */
static UmiStatus append_line(
    char *buffer,
    size_t capacity,
    size_t *length,
    const char *line,
    size_t line_length)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*length + line_length + 2U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (line_length > 0U) {
        (void)memcpy(buffer + *length, line, line_length);
        *length += line_length;
    }

    buffer[(*length)++] = '\n';
    buffer[*length] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the line equals operation used by this module and its client applications. */
static int line_equals(
    const char *line,
    size_t length,
    const char *expected)
{
    const size_t expected_length = strlen(expected);
    return length == expected_length &&
        strncmp(line, expected, length) == 0;
}

/* Provide the parse file begin operation used by this module and its client applications. */
static UmiStatus parse_file_begin(
    const char *line,
    size_t length,
    ParsedFile *file)
{
    static const char PREFIX[] = "FILE-BEGIN|";
    const size_t prefix_length = sizeof(PREFIX) - 1U;
    const char *operation;
    const char *separator;
    size_t operation_length;
    size_t path_length;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length <= prefix_length ||
        strncmp(line, PREFIX, prefix_length) != 0) {
        return UMI_STATUS_PARSE_ERROR;
    }

    operation = line + prefix_length;
    separator = memchr(operation, '|', length - prefix_length);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (separator == NULL) return UMI_STATUS_PARSE_ERROR;

    operation_length = (size_t)(separator - operation);
    path_length =
        length - prefix_length - operation_length - 1U;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (path_length == 0U || path_length >= sizeof(file->path)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (operation_length == 6U &&
        strncmp(operation, "CREATE", 6U) == 0) {
        file->operation = UMI_AI_CODING_PATCH_CREATE;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (operation_length == 6U &&
               strncmp(operation, "MODIFY", 6U) == 0) {
        file->operation = UMI_AI_CODING_PATCH_MODIFY;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (operation_length == 6U &&
               strncmp(operation, "DELETE", 6U) == 0) {
        file->operation = UMI_AI_CODING_PATCH_DELETE;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return UMI_STATUS_PARSE_ERROR;
    }

    (void)memcpy(file->path, separator + 1, path_length);
    file->path[path_length] = '\0';
    file->active = 1;
    return UMI_STATUS_OK;
}

/* Provide the add parsed file operation used by this module and its client applications. */
static UmiStatus add_parsed_file(
    UmiAiCodingPatch *patch,
    ParsedFile *file,
    const UmiAiCodingWorkspaceAdapter *workspace)
{
    char normalized[UMI_AI_TEXT_CAPACITY];
    char before[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    size_t before_length = 0U;
    int exists = 0;
    UmiStatus status;

    status = umi_ai_coding_runtime_path_normalize_relative(
        file->path,
        normalized,
        sizeof(normalized));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = workspace->exists(
        workspace->user_data,
        normalized,
        &exists);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (file->operation == UMI_AI_CODING_PATCH_CREATE) {
        /* Use the optional file only when it is present in this checkout. */
        if (exists) return UMI_STATUS_ALREADY_EXISTS;

        return umi_ai_coding_patch_add_file(
            patch,
            normalized,
            file->operation,
            "",
            file->content);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!exists) return UMI_STATUS_NOT_FOUND;

    status = workspace->read(
        workspace->user_data,
        normalized,
        before,
        sizeof(before),
        &before_length);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)before_length;

    /* Apply this branch only when its contract condition is satisfied. */
    if (file->operation == UMI_AI_CODING_PATCH_DELETE) {
        return umi_ai_coding_patch_add_file(
            patch,
            normalized,
            file->operation,
            before,
            "");
    }

    return umi_ai_coding_patch_add_file(
        patch,
        normalized,
        file->operation,
        before,
        file->content);
}

/*
 * Provide the strip trailing newline operation used by this module and its client
 * applications.
 */
static void strip_trailing_newline(char *text)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return;

    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U && text[length - 1U] == '\n') {
        text[length - 1U] = '\0';
    }
}

/*
 * Provide the ai coding parse response operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_parse_response(
    const UmiAiResponse *response,
    const UmiAiCodingTaskPlan *plan,
    const UmiAiCodingWorkspaceAdapter *workspace,
    UmiAiCodingParsedResponse *out_response)
{
    const char *cursor;
    ParseMode mode = PARSE_NORMAL;
    ParsedFile file;
    char title[UMI_AI_SMALL_TEXT_CAPACITY];
    char rationale[UMI_AI_TEXT_CAPACITY];
    size_t summary_length = 0U;
    size_t rationale_length = 0U;
    int patch_started = 0;
    int patch_finished = 0;
    char patch_id[UMI_AI_ID_CAPACITY];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (response == NULL || plan == NULL ||
        workspace == NULL || out_response == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_workspace_adapter_validate(workspace);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_response, 0, sizeof(*out_response));
    (void)memset(&file, 0, sizeof(file));
    title[0] = '\0';
    rationale[0] = '\0';

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(
            response->text,
            RESPONSE_HEADER,
            strlen(RESPONSE_HEADER)) != 0) {
        const size_t length = strlen(response->text);

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= sizeof(out_response->summary)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        (void)memcpy(
            out_response->summary,
            response->text,
            length + 1U);
        out_response->structured = 0;
        out_response->revision = plan->revision;
        return UMI_STATUS_OK;
    }

    out_response->structured = 1;

    (void)snprintf(
        patch_id,
        sizeof(patch_id),
        "patch.%016llx",
        (unsigned long long)plan->plan_hash);

    status = umi_ai_coding_patch_init(
        &out_response->patch,
        patch_id,
        plan->request.request_id,
        "AI coding change",
        "Generated by the governed Umicom coding runtime.");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    cursor = response->text;

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
        if (mode == PARSE_SUMMARY) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (line_equals(cursor, length, "SUMMARY-END")) {
                mode = PARSE_NORMAL;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                status = append_line(
                    out_response->summary,
                    sizeof(out_response->summary),
                    &summary_length,
                    cursor,
                    length);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) return status;
            }
        } else /* Apply this branch only when its contract condition is satisfied. */ if (mode == PARSE_RATIONALE) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (line_equals(cursor, length, "RATIONALE-END")) {
                mode = PARSE_NORMAL;
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
        } else /* Apply this branch only when its contract condition is satisfied. */ if (mode == PARSE_CONTENT) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (line_equals(cursor, length, "CONTENT-END")) {
                mode = PARSE_NORMAL;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                status = append_line(
                    file.content,
                    sizeof(file.content),
                    &file.content_length,
                    cursor,
                    length);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) return status;
            }
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (line_equals(cursor, length, "SUMMARY-BEGIN")) {
            mode = PARSE_SUMMARY;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (line_equals(cursor, length, "PATCH-BEGIN")) {
            patch_started = 1;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (line_equals(cursor, length, "PATCH-END")) {
            patch_finished = 1;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (length > 6U &&
                   strncmp(cursor, "TITLE|", 6U) == 0) {
            const size_t title_length = length - 6U;

            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (title_length >= sizeof(title)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }

            (void)memcpy(title, cursor + 6U, title_length);
            title[title_length] = '\0';
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (line_equals(cursor, length, "RATIONALE-BEGIN")) {
            mode = PARSE_RATIONALE;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (length > 11U &&
                   strncmp(cursor, "FILE-BEGIN|", 11U) == 0) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (file.active) return UMI_STATUS_PARSE_ERROR;
            (void)memset(&file, 0, sizeof(file));

            status = parse_file_begin(cursor, length, &file);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (line_equals(cursor, length, "CONTENT-BEGIN")) {
            /* Apply this operation only while the related capability or state is available. */
            if (!file.active ||
                file.operation == UMI_AI_CODING_PATCH_DELETE) {
                return UMI_STATUS_PARSE_ERROR;
            }
            mode = PARSE_CONTENT;
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (line_equals(cursor, length, "FILE-END")) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (!file.active) return UMI_STATUS_PARSE_ERROR;

            status = add_parsed_file(
                &out_response->patch,
                &file,
                workspace);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;

            (void)memset(&file, 0, sizeof(file));
        }

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL) break;
        cursor = end + 1;
    }

    /* Apply this operation only while the related capability or state is available. */
    if (mode != PARSE_NORMAL || file.active) {
        return UMI_STATUS_PARSE_ERROR;
    }

    strip_trailing_newline(out_response->summary);
    strip_trailing_newline(rationale);

    /* Apply this branch only when its contract condition is satisfied. */
    if (title[0] != '\0') {
        (void)snprintf(
            out_response->patch.title,
            sizeof(out_response->patch.title),
            "%s",
            title);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (rationale[0] != '\0') {
        (void)snprintf(
            out_response->patch.rationale,
            sizeof(out_response->patch.rationale),
            "%s",
            rationale);
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (patch_started != patch_finished) {
        return UMI_STATUS_PARSE_ERROR;
    }

    out_response->has_patch =
        patch_started &&
        patch_finished &&
        out_response->patch.file_count > 0U;
    out_response->revision = plan->revision + 1U;
    return UMI_STATUS_OK;
}
