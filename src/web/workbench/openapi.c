/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/openapi.c
 *
 * PURPOSE:
 *   Implement a bounded OpenAPI JSON explorer for document metadata, servers
 *   and path operations. It intentionally imports developer-facing metadata;
 *   Integration Fabric remains authoritative for executable API contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/openapi.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

typedef struct JsonRange {
    const char *begin;
    const char *end;
} JsonRange;

/* Provide the skip space operation used by this module and its client applications. */
static const char *skip_space(const char *cursor, const char *end)
{
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor < end && isspace((unsigned char)*cursor)) ++cursor;
    return cursor;
}

/* Provide the skip string operation used by this module and its client applications. */
static const char *skip_string(const char *cursor, const char *end)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (cursor >= end || *cursor != '"') return NULL;
    ++cursor;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor < end) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '\\') {
            cursor += 2;
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '"') return cursor + 1;
        ++cursor;
    }
    return NULL;
}

/* Provide the skip value operation used by this module and its client applications. */
static const char *skip_value(const char *cursor, const char *end)
{
    char opening;
    char closing;
    unsigned depth = 0U;
    cursor = skip_space(cursor, end);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (cursor >= end) return NULL;
    /* Apply this branch only when its contract condition is satisfied. */
    if (*cursor == '"') return skip_string(cursor, end);
    opening = *cursor;
    /* Apply this branch only when its contract condition is satisfied. */
    if (opening != '{' && opening != '[') {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (cursor < end && *cursor != ',' && *cursor != '}' && *cursor != ']') ++cursor;
        return cursor;
    }
    closing = opening == '{' ? '}' : ']';
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor < end) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '"') {
            cursor = skip_string(cursor, end);
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (cursor == NULL) return NULL;
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == opening) depth++;
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == closing) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (depth == 0U) return NULL;
            depth--;
            /* Apply this branch only when its contract condition is satisfied. */
            if (depth == 0U) return cursor + 1;
        }
        ++cursor;
    }
    return NULL;
}

/* Provide the parse string operation used by this module and its client applications. */
static UmiStatus parse_string(
    const char *cursor,
    const char *end,
    char *out_text,
    size_t capacity,
    const char **out_after)
{
    size_t length = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor == NULL || cursor >= end || *cursor != '"' ||
        out_text == NULL || capacity == 0U) return UMI_STATUS_PARSE_ERROR;
    ++cursor;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor < end && *cursor != '"') {
        char value = *cursor++;
        /* Apply this branch only when its contract condition is satisfied. */
        if (value == '\\') {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (cursor >= end) return UMI_STATUS_PARSE_ERROR;
            value = *cursor++;
            /* Select the behaviour associated with the requested command or state value. */
            switch (value) {
                case 'n': value = '\n'; break;
                case 'r': value = '\r'; break;
                case 't': value = '\t'; break;
                case 'b': value = '\b'; break;
                case 'f': value = '\f'; break;
                case '"': case '\\': case '/': break;
                default: return UMI_STATUS_PARSE_ERROR;
            }
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        out_text[length++] = value;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (cursor >= end || *cursor != '"') return UMI_STATUS_PARSE_ERROR;
    out_text[length] = '\0';
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_after != NULL) *out_after = cursor + 1;
    return UMI_STATUS_OK;
}

/* Provide the member next operation used by this module and its client applications. */
static int member_next(
    JsonRange object,
    const char **cursor,
    char *out_key,
    size_t key_capacity,
    JsonRange *out_value)
{
    const char *current;
    const char *after_key;
    const char *after_value;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor == NULL || *cursor == NULL || object.begin == NULL || object.end == NULL) return 0;
    current = *cursor == object.begin ? object.begin + 1 : *cursor;
    current = skip_space(current, object.end);
    /* Apply this branch only when its contract condition is satisfied. */
    if (current < object.end && *current == ',') current = skip_space(current + 1, object.end);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (current >= object.end || *current == '}') return 0;
    status = parse_string(current, object.end, out_key, key_capacity, &after_key);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return -1;
    current = skip_space(after_key, object.end);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (current >= object.end || *current != ':') return -1;
    current = skip_space(current + 1, object.end);
    after_value = skip_value(current, object.end);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (after_value == NULL) return -1;
    out_value->begin = current;
    out_value->end = after_value;
    *cursor = after_value;
    return 1;
}

/* Provide the find member operation used by this module and its client applications. */
static int find_member(JsonRange object, const char *name, JsonRange *out_value)
{
    const char *cursor = object.begin;
    char key[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    JsonRange value;
    int result;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while ((result = member_next(object, &cursor, key, sizeof(key), &value)) > 0) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(key, name) == 0) {
            *out_value = value;
            return 1;
        }
    }
    return result < 0 ? -1 : 0;
}

/* Provide the object range operation used by this module and its client applications. */
static int object_range(JsonRange value, JsonRange *out_object)
{
    const char *begin = skip_space(value.begin, value.end);
    const char *after;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (begin >= value.end || *begin != '{') return 0;
    after = skip_value(begin, value.end);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (after == NULL || after > value.end) return 0;
    out_object->begin = begin;
    out_object->end = after;
    return 1;
}

/* Provide the member string operation used by this module and its client applications. */
static UmiStatus member_string(
    JsonRange object,
    const char *name,
    char *out_text,
    size_t capacity,
    bool required)
{
    JsonRange value;
    int found = find_member(object, name, &value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (found < 0) return UMI_STATUS_PARSE_ERROR;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (found == 0) return required ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
    return parse_string(skip_space(value.begin, value.end), value.end,
        out_text, capacity, NULL);
}

/* Provide the member boolean operation used by this module and its client applications. */
static bool member_boolean(JsonRange object, const char *name)
{
    JsonRange value;
    const char *cursor;
    /* Apply this branch only when its contract condition is satisfied. */
    if (find_member(object, name, &value) != 1) return false;
    cursor = skip_space(value.begin, value.end);
    return (size_t)(value.end - cursor) >= 4U && strncmp(cursor, "true", 4U) == 0;
}

/* Provide the method from key operation used by this module and its client applications. */
static UmiHttpMethod method_from_key(const char *key)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "get") == 0) return UMI_HTTP_METHOD_GET;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "post") == 0) return UMI_HTTP_METHOD_POST;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "put") == 0) return UMI_HTTP_METHOD_PUT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "patch") == 0) return UMI_HTTP_METHOD_PATCH;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "delete") == 0) return UMI_HTTP_METHOD_DELETE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "options") == 0) return UMI_HTTP_METHOD_OPTIONS;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(key, "head") == 0) return UMI_HTTP_METHOD_HEAD;
    return UMI_HTTP_METHOD_UNKNOWN;
}

/*
 * Provide the first array string operation used by this module and its client
 * applications.
 */
static UmiStatus first_array_string(
    JsonRange object,
    const char *name,
    char *out_text,
    size_t capacity)
{
    JsonRange value;
    const char *cursor;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (find_member(object, name, &value) != 1) return UMI_STATUS_NOT_FOUND;
    cursor = skip_space(value.begin, value.end);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (cursor >= value.end || *cursor != '[') return UMI_STATUS_PARSE_ERROR;
    cursor = skip_space(cursor + 1, value.end);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (cursor >= value.end || *cursor != '"') return UMI_STATUS_NOT_FOUND;
    return parse_string(cursor, value.end, out_text, capacity, NULL);
}

/*
 * Initialise web workbench openapi from caller-provided values so later operations receive
 * a known state.
 */
void umi_web_workbench_openapi_init(
    UmiWebWorkbenchOpenApiDocument *document,
    const char *document_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL) return;
    memset(document, 0, sizeof(*document));
    (void)umi_web_workbench_copy_text(document->document_id,
        sizeof(document->document_id), document_id != NULL ? document_id : "openapi");
    document->revision = 1U;
}

/*
 * Provide the web workbench openapi add operation operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_openapi_add_operation(
    UmiWebWorkbenchOpenApiDocument *document,
    const UmiWebWorkbenchOpenApiOperation *operation)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || operation == NULL || operation->operation_id[0] == '\0' ||
        operation->path[0] != '/' || operation->method == UMI_HTTP_METHOD_UNKNOWN) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->operation_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(document->operations[index].operation_id, operation->operation_id) == 0) {
            document->operations[index] = *operation;
            document->revision++;
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (document->operation_count >= UMI_WEB_WORKBENCH_MAX_OPERATIONS) {
        document->skipped_operation_count++;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    document->operations[document->operation_count++] = *operation;
    document->revision++;
    return UMI_STATUS_OK;
}

/* Provide the import metadata operation used by this module and its client applications. */
static UmiStatus import_metadata(
    UmiWebWorkbenchOpenApiDocument *document,
    JsonRange root)
{
    JsonRange value;
    JsonRange object;
    UmiStatus status;
    status = member_string(root, "openapi", document->specification,
        sizeof(document->specification), false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && document->specification[0] == '\0') {
        status = member_string(root, "swagger", document->specification,
            sizeof(document->specification), true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (find_member(root, "info", &value) != 1 || !object_range(value, &object)) {
        return UMI_STATUS_PARSE_ERROR;
    }
    status = member_string(object, "title", document->title,
        sizeof(document->title), true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = member_string(object, "version", document->version,
        sizeof(document->version), true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (find_member(root, "servers", &value) == 1) {
        const char *cursor = skip_space(value.begin, value.end);
        /* Apply this branch only when its contract condition is satisfied. */
        if (cursor < value.end && *cursor == '[') {
            JsonRange server_value;
            JsonRange server_object;
            cursor = skip_space(cursor + 1, value.end);
            server_value.begin = cursor;
            server_value.end = value.end;
            /* Apply this branch only when its contract condition is satisfied. */
            if (object_range(server_value, &server_object)) {
                (void)member_string(server_object, "url", document->server_url,
                    sizeof(document->server_url), false);
            }
        }
    }
    return UMI_STATUS_OK;
}

/* Provide the import operation operation used by this module and its client applications. */
static UmiStatus import_operation(
    UmiWebWorkbenchOpenApiDocument *document,
    const char *path,
    const char *method_key,
    JsonRange value)
{
    JsonRange object;
    UmiWebWorkbenchOpenApiOperation operation;
    UmiStatus status;
    int written;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!object_range(value, &object)) return UMI_STATUS_PARSE_ERROR;
    memset(&operation, 0, sizeof(operation));
    operation.method = method_from_key(method_key);
    status = umi_web_workbench_copy_text(operation.path,
        sizeof(operation.path), path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = member_string(object, "operationId", operation.operation_id,
        sizeof(operation.operation_id), false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) return status;
    (void)member_string(object, "summary", operation.summary,
        sizeof(operation.summary), false);
    (void)first_array_string(object, "tags", operation.tag, sizeof(operation.tag));
    operation.deprecated = member_boolean(object, "deprecated");
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (operation.operation_id[0] == '\0') {
        written = snprintf(operation.operation_id, sizeof(operation.operation_id),
            "%s-%s", method_key, path[0] == '/' ? path + 1 : path);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(operation.operation_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        {
            char *cursor = operation.operation_id;
            /*
             * Continue only while work remains available; the loop body advances the state on each
             * pass.
             */
            while (*cursor != '\0') {
                /* Apply this branch only when its contract condition is satisfied. */
                if (*cursor == '/' || *cursor == '{' || *cursor == '}') *cursor = '-';
                ++cursor;
            }
        }
    }
    return umi_web_workbench_openapi_add_operation(document, &operation);
}

/* Provide the import paths operation used by this module and its client applications. */
static UmiStatus import_paths(
    UmiWebWorkbenchOpenApiDocument *document,
    JsonRange root)
{
    JsonRange paths_value;
    JsonRange paths_object;
    const char *path_cursor;
    char path[UMI_WEB_PATH_CAPACITY];
    JsonRange path_value;
    int result;
    /* Apply this branch only when its contract condition is satisfied. */
    if (find_member(root, "paths", &paths_value) != 1 ||
        !object_range(paths_value, &paths_object)) return UMI_STATUS_PARSE_ERROR;
    path_cursor = paths_object.begin;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while ((result = member_next(paths_object, &path_cursor, path,
            sizeof(path), &path_value)) > 0) {
        JsonRange path_object;
        const char *method_cursor;
        char method_key[32U];
        JsonRange method_value;
        int method_result;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!object_range(path_value, &path_object)) continue;
        method_cursor = path_object.begin;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while ((method_result = member_next(path_object, &method_cursor,
                method_key, sizeof(method_key), &method_value)) > 0) {
            UmiHttpMethod method = method_from_key(method_key);
            /* Apply this branch only when its contract condition is satisfied. */
            if (method != UMI_HTTP_METHOD_UNKNOWN) {
                UmiStatus status = import_operation(document, path, method_key, method_value);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK && status != UMI_STATUS_CAPACITY_EXCEEDED) {
                    return status;
                }
            }
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (method_result < 0) return UMI_STATUS_PARSE_ERROR;
    }
    return result < 0 ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
}

/*
 * Provide the web workbench openapi import json operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_openapi_import_json(
    UmiWebWorkbenchOpenApiDocument *document,
    const char *json_text,
    char *out_message,
    size_t message_capacity)
{
    JsonRange root;
    const char *end;
    UmiWebWorkbenchOpenApiDocument imported;
    UmiStatus status;
    const char *message;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || json_text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    end = json_text + strlen(json_text);
    root.begin = skip_space(json_text, end);
    root.end = skip_value(root.begin, end);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root.end == NULL || root.begin >= end || *root.begin != '{') {
        status = UMI_STATUS_PARSE_ERROR;
        message = "OpenAPI source is not a complete JSON object.";
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        umi_web_workbench_openapi_init(&imported, document->document_id);
        status = import_metadata(&imported, root);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = import_paths(&imported, root);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK && imported.operation_count == 0U) {
            status = UMI_STATUS_NOT_FOUND;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            imported.revision = document->revision + 1U;
            *document = imported;
            message = "OpenAPI document imported successfully.";
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            message = "OpenAPI document metadata or paths could not be imported.";
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_message != NULL && message_capacity > 0U) {
        UmiStatus copy_status = umi_web_workbench_copy_text(
            out_message, message_capacity, message);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (copy_status != UMI_STATUS_OK && status == UMI_STATUS_OK) return copy_status;
    }
    return status;
}

/*
 * Find web workbench openapi while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiWebWorkbenchOpenApiOperation *umi_web_workbench_openapi_find(
    const UmiWebWorkbenchOpenApiDocument *document,
    const char *operation_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || operation_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->operation_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(document->operations[index].operation_id, operation_id) == 0) {
            return &document->operations[index];
        }
    }
    return NULL;
}

/*
 * Provide the contains case insensitive operation used by this module and its client
 * applications.
 */
static int contains_case_insensitive(const char *text, const char *query)
{
    size_t length;
    const char *cursor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || query[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    length = strlen(query);
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = text; *cursor != '\0'; ++cursor) {
        size_t index;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < length; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (cursor[index] == '\0' ||
                tolower((unsigned char)cursor[index]) !=
                tolower((unsigned char)query[index])) break;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index == length) return 1;
    }
    return 0;
}

/*
 * Provide the web workbench openapi query operation used by this module and its client
 * applications.
 */
size_t umi_web_workbench_openapi_query(
    const UmiWebWorkbenchOpenApiDocument *document,
    const char *text,
    const char *tag,
    const UmiWebWorkbenchOpenApiOperation **out_operations,
    size_t capacity)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || (out_operations == NULL && capacity > 0U)) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->operation_count && count < capacity; ++index) {
        const UmiWebWorkbenchOpenApiOperation *operation = &document->operations[index];
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (tag != NULL && tag[0] != '\0' && strcmp(operation->tag, tag) != 0) continue;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!contains_case_insensitive(operation->operation_id, text) &&
            !contains_case_insensitive(operation->summary, text) &&
            !contains_case_insensitive(operation->path, text)) continue;
        out_operations[count++] = operation;
    }
    return count;
}

/*
 * Provide the web workbench openapi make request operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_openapi_make_request(
    const UmiWebWorkbenchOpenApiDocument *document,
    const char *operation_id,
    UmiWebWorkbenchRequest *out_request)
{
    const UmiWebWorkbenchOpenApiOperation *operation;
    char url[UMI_WEB_WORKBENCH_URL_CAPACITY];
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || operation_id == NULL || out_request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    operation = umi_web_workbench_openapi_find(document, operation_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operation == NULL) return UMI_STATUS_NOT_FOUND;
    written = snprintf(url, sizeof(url), "%s%s",
        document->server_url[0] != '\0' ? document->server_url : "{{baseUrl}}",
        operation->path);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(url)) return UMI_STATUS_CAPACITY_EXCEEDED;
    umi_web_workbench_request_init(out_request, operation->operation_id,
        operation->summary[0] != '\0' ? operation->summary : operation->operation_id,
        operation->method, url);
    return UMI_STATUS_OK;
}
