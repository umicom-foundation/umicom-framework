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

static const char *skip_space(const char *cursor, const char *end)
{
    while (cursor < end && isspace((unsigned char)*cursor)) ++cursor;
    return cursor;
}

static const char *skip_string(const char *cursor, const char *end)
{
    if (cursor >= end || *cursor != '"') return NULL;
    ++cursor;
    while (cursor < end) {
        if (*cursor == '\\') {
            cursor += 2;
            continue;
        }
        if (*cursor == '"') return cursor + 1;
        ++cursor;
    }
    return NULL;
}

static const char *skip_value(const char *cursor, const char *end)
{
    char opening;
    char closing;
    unsigned depth = 0U;
    cursor = skip_space(cursor, end);
    if (cursor >= end) return NULL;
    if (*cursor == '"') return skip_string(cursor, end);
    opening = *cursor;
    if (opening != '{' && opening != '[') {
        while (cursor < end && *cursor != ',' && *cursor != '}' && *cursor != ']') ++cursor;
        return cursor;
    }
    closing = opening == '{' ? '}' : ']';
    while (cursor < end) {
        if (*cursor == '"') {
            cursor = skip_string(cursor, end);
            if (cursor == NULL) return NULL;
            continue;
        }
        if (*cursor == opening) depth++;
        if (*cursor == closing) {
            if (depth == 0U) return NULL;
            depth--;
            if (depth == 0U) return cursor + 1;
        }
        ++cursor;
    }
    return NULL;
}

static UmiStatus parse_string(
    const char *cursor,
    const char *end,
    char *out_text,
    size_t capacity,
    const char **out_after)
{
    size_t length = 0U;
    if (cursor == NULL || cursor >= end || *cursor != '"' ||
        out_text == NULL || capacity == 0U) return UMI_STATUS_PARSE_ERROR;
    ++cursor;
    while (cursor < end && *cursor != '"') {
        char value = *cursor++;
        if (value == '\\') {
            if (cursor >= end) return UMI_STATUS_PARSE_ERROR;
            value = *cursor++;
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
        if (length + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        out_text[length++] = value;
    }
    if (cursor >= end || *cursor != '"') return UMI_STATUS_PARSE_ERROR;
    out_text[length] = '\0';
    if (out_after != NULL) *out_after = cursor + 1;
    return UMI_STATUS_OK;
}

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
    if (cursor == NULL || *cursor == NULL || object.begin == NULL || object.end == NULL) return 0;
    current = *cursor == object.begin ? object.begin + 1 : *cursor;
    current = skip_space(current, object.end);
    if (current < object.end && *current == ',') current = skip_space(current + 1, object.end);
    if (current >= object.end || *current == '}') return 0;
    status = parse_string(current, object.end, out_key, key_capacity, &after_key);
    if (status != UMI_STATUS_OK) return -1;
    current = skip_space(after_key, object.end);
    if (current >= object.end || *current != ':') return -1;
    current = skip_space(current + 1, object.end);
    after_value = skip_value(current, object.end);
    if (after_value == NULL) return -1;
    out_value->begin = current;
    out_value->end = after_value;
    *cursor = after_value;
    return 1;
}

static int find_member(JsonRange object, const char *name, JsonRange *out_value)
{
    const char *cursor = object.begin;
    char key[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    JsonRange value;
    int result;
    while ((result = member_next(object, &cursor, key, sizeof(key), &value)) > 0) {
        if (strcmp(key, name) == 0) {
            *out_value = value;
            return 1;
        }
    }
    return result < 0 ? -1 : 0;
}

static int object_range(JsonRange value, JsonRange *out_object)
{
    const char *begin = skip_space(value.begin, value.end);
    const char *after;
    if (begin >= value.end || *begin != '{') return 0;
    after = skip_value(begin, value.end);
    if (after == NULL || after > value.end) return 0;
    out_object->begin = begin;
    out_object->end = after;
    return 1;
}

static UmiStatus member_string(
    JsonRange object,
    const char *name,
    char *out_text,
    size_t capacity,
    bool required)
{
    JsonRange value;
    int found = find_member(object, name, &value);
    if (found < 0) return UMI_STATUS_PARSE_ERROR;
    if (found == 0) return required ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
    return parse_string(skip_space(value.begin, value.end), value.end,
        out_text, capacity, NULL);
}

static bool member_boolean(JsonRange object, const char *name)
{
    JsonRange value;
    const char *cursor;
    if (find_member(object, name, &value) != 1) return false;
    cursor = skip_space(value.begin, value.end);
    return (size_t)(value.end - cursor) >= 4U && strncmp(cursor, "true", 4U) == 0;
}

static UmiHttpMethod method_from_key(const char *key)
{
    if (strcmp(key, "get") == 0) return UMI_HTTP_METHOD_GET;
    if (strcmp(key, "post") == 0) return UMI_HTTP_METHOD_POST;
    if (strcmp(key, "put") == 0) return UMI_HTTP_METHOD_PUT;
    if (strcmp(key, "patch") == 0) return UMI_HTTP_METHOD_PATCH;
    if (strcmp(key, "delete") == 0) return UMI_HTTP_METHOD_DELETE;
    if (strcmp(key, "options") == 0) return UMI_HTTP_METHOD_OPTIONS;
    if (strcmp(key, "head") == 0) return UMI_HTTP_METHOD_HEAD;
    return UMI_HTTP_METHOD_UNKNOWN;
}

static UmiStatus first_array_string(
    JsonRange object,
    const char *name,
    char *out_text,
    size_t capacity)
{
    JsonRange value;
    const char *cursor;
    if (find_member(object, name, &value) != 1) return UMI_STATUS_NOT_FOUND;
    cursor = skip_space(value.begin, value.end);
    if (cursor >= value.end || *cursor != '[') return UMI_STATUS_PARSE_ERROR;
    cursor = skip_space(cursor + 1, value.end);
    if (cursor >= value.end || *cursor != '"') return UMI_STATUS_NOT_FOUND;
    return parse_string(cursor, value.end, out_text, capacity, NULL);
}

void umi_web_workbench_openapi_init(
    UmiWebWorkbenchOpenApiDocument *document,
    const char *document_id)
{
    if (document == NULL) return;
    memset(document, 0, sizeof(*document));
    (void)umi_web_workbench_copy_text(document->document_id,
        sizeof(document->document_id), document_id != NULL ? document_id : "openapi");
    document->revision = 1U;
}

UmiStatus umi_web_workbench_openapi_add_operation(
    UmiWebWorkbenchOpenApiDocument *document,
    const UmiWebWorkbenchOpenApiOperation *operation)
{
    size_t index;
    if (document == NULL || operation == NULL || operation->operation_id[0] == '\0' ||
        operation->path[0] != '/' || operation->method == UMI_HTTP_METHOD_UNKNOWN) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < document->operation_count; ++index) {
        if (strcmp(document->operations[index].operation_id, operation->operation_id) == 0) {
            document->operations[index] = *operation;
            document->revision++;
            return UMI_STATUS_OK;
        }
    }
    if (document->operation_count >= UMI_WEB_WORKBENCH_MAX_OPERATIONS) {
        document->skipped_operation_count++;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    document->operations[document->operation_count++] = *operation;
    document->revision++;
    return UMI_STATUS_OK;
}

static UmiStatus import_metadata(
    UmiWebWorkbenchOpenApiDocument *document,
    JsonRange root)
{
    JsonRange value;
    JsonRange object;
    UmiStatus status;
    status = member_string(root, "openapi", document->specification,
        sizeof(document->specification), false);
    if (status == UMI_STATUS_OK && document->specification[0] == '\0') {
        status = member_string(root, "swagger", document->specification,
            sizeof(document->specification), true);
    }
    if (status != UMI_STATUS_OK) return status;
    if (find_member(root, "info", &value) != 1 || !object_range(value, &object)) {
        return UMI_STATUS_PARSE_ERROR;
    }
    status = member_string(object, "title", document->title,
        sizeof(document->title), true);
    if (status != UMI_STATUS_OK) return status;
    status = member_string(object, "version", document->version,
        sizeof(document->version), true);
    if (status != UMI_STATUS_OK) return status;
    if (find_member(root, "servers", &value) == 1) {
        const char *cursor = skip_space(value.begin, value.end);
        if (cursor < value.end && *cursor == '[') {
            JsonRange server_value;
            JsonRange server_object;
            cursor = skip_space(cursor + 1, value.end);
            server_value.begin = cursor;
            server_value.end = value.end;
            if (object_range(server_value, &server_object)) {
                (void)member_string(server_object, "url", document->server_url,
                    sizeof(document->server_url), false);
            }
        }
    }
    return UMI_STATUS_OK;
}

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
    if (!object_range(value, &object)) return UMI_STATUS_PARSE_ERROR;
    memset(&operation, 0, sizeof(operation));
    operation.method = method_from_key(method_key);
    status = umi_web_workbench_copy_text(operation.path,
        sizeof(operation.path), path);
    if (status != UMI_STATUS_OK) return status;
    status = member_string(object, "operationId", operation.operation_id,
        sizeof(operation.operation_id), false);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) return status;
    (void)member_string(object, "summary", operation.summary,
        sizeof(operation.summary), false);
    (void)first_array_string(object, "tags", operation.tag, sizeof(operation.tag));
    operation.deprecated = member_boolean(object, "deprecated");
    if (operation.operation_id[0] == '\0') {
        written = snprintf(operation.operation_id, sizeof(operation.operation_id),
            "%s-%s", method_key, path[0] == '/' ? path + 1 : path);
        if (written < 0 || (size_t)written >= sizeof(operation.operation_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        {
            char *cursor = operation.operation_id;
            while (*cursor != '\0') {
                if (*cursor == '/' || *cursor == '{' || *cursor == '}') *cursor = '-';
                ++cursor;
            }
        }
    }
    return umi_web_workbench_openapi_add_operation(document, &operation);
}

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
    if (find_member(root, "paths", &paths_value) != 1 ||
        !object_range(paths_value, &paths_object)) return UMI_STATUS_PARSE_ERROR;
    path_cursor = paths_object.begin;
    while ((result = member_next(paths_object, &path_cursor, path,
            sizeof(path), &path_value)) > 0) {
        JsonRange path_object;
        const char *method_cursor;
        char method_key[32U];
        JsonRange method_value;
        int method_result;
        if (!object_range(path_value, &path_object)) continue;
        method_cursor = path_object.begin;
        while ((method_result = member_next(path_object, &method_cursor,
                method_key, sizeof(method_key), &method_value)) > 0) {
            UmiHttpMethod method = method_from_key(method_key);
            if (method != UMI_HTTP_METHOD_UNKNOWN) {
                UmiStatus status = import_operation(document, path, method_key, method_value);
                if (status != UMI_STATUS_OK && status != UMI_STATUS_CAPACITY_EXCEEDED) {
                    return status;
                }
            }
        }
        if (method_result < 0) return UMI_STATUS_PARSE_ERROR;
    }
    return result < 0 ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
}

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
    if (document == NULL || json_text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    end = json_text + strlen(json_text);
    root.begin = skip_space(json_text, end);
    root.end = skip_value(root.begin, end);
    if (root.end == NULL || root.begin >= end || *root.begin != '{') {
        status = UMI_STATUS_PARSE_ERROR;
        message = "OpenAPI source is not a complete JSON object.";
    } else {
        umi_web_workbench_openapi_init(&imported, document->document_id);
        status = import_metadata(&imported, root);
        if (status == UMI_STATUS_OK) status = import_paths(&imported, root);
        if (status == UMI_STATUS_OK && imported.operation_count == 0U) {
            status = UMI_STATUS_NOT_FOUND;
        }
        if (status == UMI_STATUS_OK) {
            imported.revision = document->revision + 1U;
            *document = imported;
            message = "OpenAPI document imported successfully.";
        } else {
            message = "OpenAPI document metadata or paths could not be imported.";
        }
    }
    if (out_message != NULL && message_capacity > 0U) {
        UmiStatus copy_status = umi_web_workbench_copy_text(
            out_message, message_capacity, message);
        if (copy_status != UMI_STATUS_OK && status == UMI_STATUS_OK) return copy_status;
    }
    return status;
}

const UmiWebWorkbenchOpenApiOperation *umi_web_workbench_openapi_find(
    const UmiWebWorkbenchOpenApiDocument *document,
    const char *operation_id)
{
    size_t index;
    if (document == NULL || operation_id == NULL) return NULL;
    for (index = 0U; index < document->operation_count; ++index) {
        if (strcmp(document->operations[index].operation_id, operation_id) == 0) {
            return &document->operations[index];
        }
    }
    return NULL;
}

static int contains_case_insensitive(const char *text, const char *query)
{
    size_t length;
    const char *cursor;
    if (query == NULL || query[0] == '\0') return 1;
    if (text == NULL) return 0;
    length = strlen(query);
    for (cursor = text; *cursor != '\0'; ++cursor) {
        size_t index;
        for (index = 0U; index < length; ++index) {
            if (cursor[index] == '\0' ||
                tolower((unsigned char)cursor[index]) !=
                tolower((unsigned char)query[index])) break;
        }
        if (index == length) return 1;
    }
    return 0;
}

size_t umi_web_workbench_openapi_query(
    const UmiWebWorkbenchOpenApiDocument *document,
    const char *text,
    const char *tag,
    const UmiWebWorkbenchOpenApiOperation **out_operations,
    size_t capacity)
{
    size_t index;
    size_t count = 0U;
    if (document == NULL || (out_operations == NULL && capacity > 0U)) return 0U;
    for (index = 0U; index < document->operation_count && count < capacity; ++index) {
        const UmiWebWorkbenchOpenApiOperation *operation = &document->operations[index];
        if (tag != NULL && tag[0] != '\0' && strcmp(operation->tag, tag) != 0) continue;
        if (!contains_case_insensitive(operation->operation_id, text) &&
            !contains_case_insensitive(operation->summary, text) &&
            !contains_case_insensitive(operation->path, text)) continue;
        out_operations[count++] = operation;
    }
    return count;
}

UmiStatus umi_web_workbench_openapi_make_request(
    const UmiWebWorkbenchOpenApiDocument *document,
    const char *operation_id,
    UmiWebWorkbenchRequest *out_request)
{
    const UmiWebWorkbenchOpenApiOperation *operation;
    char url[UMI_WEB_WORKBENCH_URL_CAPACITY];
    int written;
    if (document == NULL || operation_id == NULL || out_request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    operation = umi_web_workbench_openapi_find(document, operation_id);
    if (operation == NULL) return UMI_STATUS_NOT_FOUND;
    written = snprintf(url, sizeof(url), "%s%s",
        document->server_url[0] != '\0' ? document->server_url : "{{baseUrl}}",
        operation->path);
    if (written < 0 || (size_t)written >= sizeof(url)) return UMI_STATUS_CAPACITY_EXCEEDED;
    umi_web_workbench_request_init(out_request, operation->operation_id,
        operation->summary[0] != '\0' ? operation->summary : operation->operation_id,
        operation->method, url);
    return UMI_STATUS_OK;
}
