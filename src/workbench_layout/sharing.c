/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/sharing.c
 *
 * PURPOSE:
 *   Implement portable semantic layout bundles with explicit provenance,
 *   integrity evidence and controlled identity transfer during import.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout/sharing.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

#define UMI_SHARE_SCHEMA "umicom.workbench-layout-bundle/1"

typedef struct UmiShareWriter {
    char *buffer;
    size_t capacity;
    size_t written;
    size_t required;
    bool overflow;
} UmiShareWriter;

static void share_write_bytes(
    UmiShareWriter *writer,
    const char *text,
    size_t length)
{
    size_t available;
    size_t copy_count;
    writer->required += length;
    if (writer->buffer == NULL || writer->capacity == 0U) {
        writer->overflow = true;
        return;
    }
    available = writer->written < writer->capacity
        ? writer->capacity - writer->written
        : 0U;
    if (available == 0U) {
        writer->overflow = true;
        return;
    }
    copy_count = length < available - 1U ? length : available - 1U;
    if (copy_count > 0U) {
        (void)memcpy(writer->buffer + writer->written, text, copy_count);
        writer->written += copy_count;
    }
    writer->buffer[writer->written] = '\0';
    if (copy_count != length) writer->overflow = true;
}

static void share_write(UmiShareWriter *writer, const char *text)
{
    share_write_bytes(writer, text, strlen(text));
}

static void share_format(UmiShareWriter *writer, const char *format, ...)
{
    char temporary[512U];
    va_list arguments;
    int written;
    va_start(arguments, format);
    written = vsnprintf(temporary, sizeof(temporary), format, arguments);
    va_end(arguments);
    if (written < 0) {
        writer->overflow = true;
        return;
    }
    if ((size_t)written >= sizeof(temporary)) {
        writer->overflow = true;
        writer->required += (size_t)written;
        return;
    }
    share_write_bytes(writer, temporary, (size_t)written);
}

static UmiStatus escaped_text_allocate(
    const char *text,
    char **out_text,
    size_t *out_length)
{
    size_t required = 0U;
    char *escaped;
    UmiStatus status;

    status = umi_workbench_layout_json_escape(
        text != NULL ? text : "", NULL, 0U, &required);
    if (status != UMI_STATUS_CAPACITY_EXCEEDED &&
        status != UMI_STATUS_OK) {
        return status;
    }
    escaped = (char *)malloc(required);
    if (escaped == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_workbench_layout_json_escape(
        text != NULL ? text : "", escaped, required, &required);
    if (status != UMI_STATUS_OK) {
        free(escaped);
        return status;
    }
    *out_text = escaped;
    if (out_length != NULL) *out_length = strlen(escaped);
    return UMI_STATUS_OK;
}


static UmiStatus share_write_json_string(
    UmiShareWriter *writer,
    const char *text)
{
    char *escaped = NULL;
    size_t escaped_length = 0U;
    UmiStatus status;

    if (writer == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = escaped_text_allocate(
        text != NULL ? text : "",
        &escaped,
        &escaped_length);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    share_write_bytes(writer, escaped, escaped_length);
    free(escaped);
    return UMI_STATUS_OK;
}

static void strip_runtime_geometry(UmiWorkbenchLayoutDocument *document)
{
    size_t index;
    for (index = 0U; index < document->node_count; ++index) {
        document->nodes[index].monitor_id[0] = '\0';
        (void)memset(
            &document->nodes[index].bounds,
            0,
            sizeof(document->nodes[index].bounds));
        document->nodes[index].z_order = 0;
    }
}

static void strip_audit(UmiWorkbenchLayoutDocument *document)
{
    (void)memset(&document->audit, 0, sizeof(document->audit));
}

UmiWorkbenchLayoutImportOptions
umi_workbench_layout_import_options_default(void)
{
    UmiWorkbenchLayoutImportOptions options;
    (void)memset(&options, 0, sizeof(options));
    options.structure_size = sizeof(options);
    options.preserve_source_audit = false;
    options.preserve_runtime_geometry = true;
    options.mark_shared = true;
    options.require_integrity_match = true;
    return options;
}

UmiStatus umi_workbench_layout_share_bundle_create(
    const UmiWorkbenchLayoutDocument *document,
    const char *bundle_id,
    const char *exported_by,
    uint64_t exported_at_ms,
    bool include_runtime_geometry,
    bool include_audit,
    UmiWorkbenchLayoutShareBundle *out_bundle)
{
    UmiStatus status;

    if (document == NULL || out_bundle == NULL ||
        !umi_workbench_layout_text_present(bundle_id) ||
        !umi_workbench_layout_text_present(exported_by)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_document_validate_structure(document);
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_bundle, 0, sizeof(*out_bundle));
    out_bundle->structure_size = sizeof(*out_bundle);
    out_bundle->manifest.structure_size = sizeof(out_bundle->manifest);
    status = umi_workbench_layout_document_copy(
        &out_bundle->document, document);
    if (status != UMI_STATUS_OK) return status;
    if (!include_runtime_geometry) strip_runtime_geometry(&out_bundle->document);
    if (!include_audit) strip_audit(&out_bundle->document);
    umi_workbench_layout_document_refresh_hash(&out_bundle->document);

    (void)umi_workbench_layout_copy_text(
        out_bundle->manifest.bundle_id,
        sizeof(out_bundle->manifest.bundle_id),
        bundle_id,
        false);
    (void)umi_workbench_layout_copy_text(
        out_bundle->manifest.layout_id,
        sizeof(out_bundle->manifest.layout_id),
        document->identity.layout_id,
        false);
    (void)umi_workbench_layout_copy_text(
        out_bundle->manifest.layout_name,
        sizeof(out_bundle->manifest.layout_name),
        document->name,
        false);
    (void)umi_workbench_layout_copy_text(
        out_bundle->manifest.source_owner_user_id,
        sizeof(out_bundle->manifest.source_owner_user_id),
        document->identity.owner_user_id,
        true);
    (void)umi_workbench_layout_copy_text(
        out_bundle->manifest.source_owner_application_id,
        sizeof(out_bundle->manifest.source_owner_application_id),
        document->identity.owner_application_id,
        true);
    (void)umi_workbench_layout_copy_text(
        out_bundle->manifest.source_workspace_id,
        sizeof(out_bundle->manifest.source_workspace_id),
        document->identity.workspace_id,
        true);
    (void)umi_workbench_layout_copy_text(
        out_bundle->manifest.exported_by,
        sizeof(out_bundle->manifest.exported_by),
        exported_by,
        false);
    out_bundle->manifest.exported_at_ms = exported_at_ms;
    out_bundle->manifest.layout_revision = document->version.revision;
    out_bundle->manifest.content_hash = out_bundle->document.content_hash;
    out_bundle->manifest.schema_version =
        out_bundle->document.version.schema_version;
    out_bundle->manifest.includes_runtime_geometry =
        include_runtime_geometry;
    out_bundle->manifest.includes_audit = include_audit;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_share_bundle_validate(
    const UmiWorkbenchLayoutShareBundle *bundle)
{
    uint64_t calculated;
    UmiStatus status;

    if (bundle == NULL ||
        bundle->structure_size < sizeof(*bundle) ||
        bundle->manifest.structure_size < sizeof(bundle->manifest) ||
        !umi_workbench_layout_text_present(bundle->manifest.bundle_id) ||
        !umi_workbench_layout_text_present(bundle->manifest.layout_id) ||
        !umi_workbench_layout_text_present(bundle->manifest.layout_name) ||
        !umi_workbench_layout_text_present(bundle->manifest.exported_by)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_document_validate_structure(
        &bundle->document);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(
            bundle->manifest.layout_id,
            bundle->document.identity.layout_id) != 0 ||
        bundle->manifest.layout_revision !=
            bundle->document.version.revision ||
        bundle->manifest.schema_version !=
            bundle->document.version.schema_version) {
        return UMI_STATUS_INVALID_STATE;
    }
    calculated = umi_workbench_layout_document_calculate_hash(
        &bundle->document);
    if (calculated != bundle->manifest.content_hash ||
        calculated != bundle->document.content_hash) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_share_bundle_import(
    const UmiWorkbenchLayoutShareBundle *bundle,
    const UmiWorkbenchLayoutImportOptions *options,
    UmiWorkbenchLayoutDocument *out_document)
{
    UmiWorkbenchLayoutImportOptions effective;
    UmiStatus status;

    if (bundle == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective = options != NULL
        ? *options
        : umi_workbench_layout_import_options_default();
    if (effective.structure_size < sizeof(effective)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (effective.require_integrity_match) {
        status = umi_workbench_layout_share_bundle_validate(bundle);
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_workbench_layout_document_copy(
        out_document, &bundle->document);
    if (status != UMI_STATUS_OK) return status;

    if (umi_workbench_layout_text_present(effective.target_layout_id)) {
        (void)umi_workbench_layout_copy_text(
            out_document->identity.layout_id,
            sizeof(out_document->identity.layout_id),
            effective.target_layout_id,
            false);
    }
    if (umi_workbench_layout_text_present(effective.target_owner_user_id)) {
        (void)umi_workbench_layout_copy_text(
            out_document->identity.owner_user_id,
            sizeof(out_document->identity.owner_user_id),
            effective.target_owner_user_id,
            false);
    }
    if (umi_workbench_layout_text_present(
            effective.target_owner_application_id)) {
        (void)umi_workbench_layout_copy_text(
            out_document->identity.owner_application_id,
            sizeof(out_document->identity.owner_application_id),
            effective.target_owner_application_id,
            false);
    }
    if (umi_workbench_layout_text_present(effective.target_workspace_id)) {
        (void)umi_workbench_layout_copy_text(
            out_document->identity.workspace_id,
            sizeof(out_document->identity.workspace_id),
            effective.target_workspace_id,
            false);
    }
    if (!effective.preserve_source_audit) strip_audit(out_document);
    if (!effective.preserve_runtime_geometry) strip_runtime_geometry(out_document);
    (void)umi_workbench_layout_document_set_flag(
        out_document,
        UMI_WORKBENCH_LAYOUT_DOCUMENT_SHARED,
        effective.mark_shared);
    (void)umi_workbench_layout_document_set_flag(
        out_document,
        UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY,
        true);
    (void)umi_workbench_layout_document_set_flag(
        out_document,
        UMI_WORKBENCH_LAYOUT_DOCUMENT_RECOVERED,
        false);
    out_document->version.base_revision = 0U;
    out_document->version.revision = 1U;
    out_document->version.generation += 1U;
    umi_workbench_layout_document_refresh_hash(out_document);
    return umi_workbench_layout_document_validate_structure(out_document);
}

UmiStatus umi_workbench_layout_share_bundle_encode(
    const UmiWorkbenchLayoutShareBundle *bundle,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    UmiWorkbenchLayoutJsonOptions options;
    UmiWorkbenchLayoutJsonResult json_result;
    UmiShareWriter writer;
    char *document_json;
    char *signature;
    size_t document_capacity;
    size_t signature_length;
    UmiStatus status;

    if (bundle == NULL || out_required == NULL ||
        (buffer == NULL && capacity != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_share_bundle_validate(bundle);
    if (status != UMI_STATUS_OK) return status;

    options = umi_workbench_layout_json_options_default();
    options.pretty = false;
    options.include_audit = bundle->manifest.includes_audit;
    options.include_runtime_geometry =
        bundle->manifest.includes_runtime_geometry;
    status = umi_workbench_layout_json_encode(
        &bundle->document,
        &options,
        NULL,
        0U,
        &json_result);
    if (status != UMI_STATUS_CAPACITY_EXCEEDED &&
        status != UMI_STATUS_OK) {
        return status;
    }
    document_capacity = json_result.bytes_required;
    document_json = (char *)malloc(document_capacity);
    if (document_json == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_workbench_layout_json_encode(
        &bundle->document,
        &options,
        document_json,
        document_capacity,
        &json_result);
    if (status != UMI_STATUS_OK) {
        free(document_json);
        return status;
    }
    status = escaped_text_allocate(
        bundle->signature_present ? bundle->signature : "",
        &signature,
        &signature_length);
    if (status != UMI_STATUS_OK) {
        free(document_json);
        return status;
    }

    (void)memset(&writer, 0, sizeof(writer));
    writer.buffer = buffer;
    writer.capacity = capacity;
    if (buffer != NULL && capacity > 0U) buffer[0] = '\0';
    share_write(&writer, "{\n  \"schema\": ");
    status = share_write_json_string(&writer, UMI_SHARE_SCHEMA);
    if (status != UMI_STATUS_OK) {
        free(signature);
        free(document_json);
        return status;
    }
    share_write(&writer, ",\n  \"manifest\": {\n");

    share_write(&writer, "    \"bundle_id\": ");
    status = share_write_json_string(&writer, bundle->manifest.bundle_id);
    if (status == UMI_STATUS_OK) {
        share_write(&writer, ",\n    \"layout_id\": ");
        status = share_write_json_string(&writer, bundle->manifest.layout_id);
    }
    if (status == UMI_STATUS_OK) {
        share_write(&writer, ",\n    \"layout_name\": ");
        status = share_write_json_string(&writer, bundle->manifest.layout_name);
    }
    if (status == UMI_STATUS_OK) {
        share_write(&writer, ",\n    \"source_owner_user_id\": ");
        status = share_write_json_string(
            &writer, bundle->manifest.source_owner_user_id);
    }
    if (status == UMI_STATUS_OK) {
        share_write(
            &writer, ",\n    \"source_owner_application_id\": ");
        status = share_write_json_string(
            &writer, bundle->manifest.source_owner_application_id);
    }
    if (status == UMI_STATUS_OK) {
        share_write(&writer, ",\n    \"source_workspace_id\": ");
        status = share_write_json_string(
            &writer, bundle->manifest.source_workspace_id);
    }
    if (status == UMI_STATUS_OK) {
        share_write(&writer, ",\n    \"exported_by\": ");
        status = share_write_json_string(&writer, bundle->manifest.exported_by);
    }
    if (status != UMI_STATUS_OK) {
        free(signature);
        free(document_json);
        return status;
    }
    share_format(
        &writer,
        ",\n    \"exported_at_ms\": %" PRIu64 ",\n"
        "    \"layout_revision\": %" PRIu64 ",\n"
        "    \"content_hash\": \"%" PRIu64 "\",\n"
        "    \"schema_version\": %" PRIu32 ",\n"
        "    \"includes_runtime_geometry\": %s,\n"
        "    \"includes_audit\": %s\n",
        bundle->manifest.exported_at_ms,
        bundle->manifest.layout_revision,
        bundle->manifest.content_hash,
        bundle->manifest.schema_version,
        bundle->manifest.includes_runtime_geometry ? "true" : "false",
        bundle->manifest.includes_audit ? "true" : "false");
    share_write(&writer, "  },\n  \"signature_present\": ");
    share_write(
        &writer, bundle->signature_present ? "true" : "false");
    share_write(&writer, ",\n  \"signature\": ");
    share_write_bytes(&writer, signature, signature_length);
    share_write(&writer, ",\n  \"document\": ");
    share_write(&writer, document_json);
    share_write(&writer, "\n}\n");

    *out_required = writer.required + 1U;
    free(signature);
    free(document_json);
    return writer.overflow
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

static const char *find_key(
    const char *text,
    size_t length,
    const char *key)
{
    char pattern[UMI_WORKBENCH_LAYOUT_ID_CAPACITY + 8U];
    const size_t pattern_length = strlen(key) + 2U;
    size_t index;

    if (strlen(key) + 3U >= sizeof(pattern)) return NULL;
    pattern[0] = '\"';
    (void)memcpy(pattern + 1U, key, strlen(key));
    pattern[strlen(key) + 1U] = '\"';
    pattern[strlen(key) + 2U] = '\0';
    for (index = 0U; index + pattern_length <= length; ++index) {
        if (memcmp(text + index, pattern, pattern_length) == 0) {
            const char *cursor = text + index + pattern_length;
            const char *end = text + length;
            while (cursor < end &&
                   (*cursor == ' ' || *cursor == '\t' ||
                    *cursor == '\n' || *cursor == '\r')) {
                cursor += 1;
            }
            if (cursor < end && *cursor == ':') {
                cursor += 1;
                while (cursor < end &&
                       (*cursor == ' ' || *cursor == '\t' ||
                        *cursor == '\n' || *cursor == '\r')) {
                    cursor += 1;
                }
                return cursor;
            }
        }
    }
    return NULL;
}

static UmiStatus read_string_field(
    const char *text,
    size_t length,
    const char *key,
    char *buffer,
    size_t capacity,
    bool required)
{
    const char *value = find_key(text, length, key);
    const char *cursor;
    bool escaped = false;
    size_t encoded_length;
    size_t required_size;

    if (value == NULL) {
        if (!required) {
            if (buffer != NULL && capacity > 0U) buffer[0] = '\0';
            return UMI_STATUS_OK;
        }
        return UMI_STATUS_NOT_FOUND;
    }
    if (*value != '\"') return UMI_STATUS_PARSE_ERROR;
    cursor = value + 1;
    while ((size_t)(cursor - text) < length) {
        if (!escaped && *cursor == '\"') break;
        if (!escaped && *cursor == '\\') escaped = true;
        else escaped = false;
        cursor += 1;
    }
    if ((size_t)(cursor - text) >= length || *cursor != '\"') {
        return UMI_STATUS_PARSE_ERROR;
    }
    encoded_length = (size_t)(cursor - (value + 1));
    return umi_workbench_layout_json_unescape(
        value + 1,
        encoded_length,
        buffer,
        capacity,
        &required_size);
}

static UmiStatus read_u64_field(
    const char *text,
    size_t length,
    const char *key,
    uint64_t *out_value)
{
    const char *value = find_key(text, length, key);
    char token[96U];
    size_t count = 0U;
    char *end;

    if (value == NULL || out_value == NULL) return UMI_STATUS_NOT_FOUND;
    if (*value == '\"') value += 1;
    while ((size_t)(value - text) < length && count + 1U < sizeof(token) &&
           *value >= '0' && *value <= '9') {
        token[count++] = *value++;
    }
    token[count] = '\0';
    if (count == 0U) return UMI_STATUS_PARSE_ERROR;
    end = NULL;
    *out_value = (uint64_t)strtoull(token, &end, 10);
    return end != token && *end == '\0'
        ? UMI_STATUS_OK
        : UMI_STATUS_PARSE_ERROR;
}

static UmiStatus read_bool_field(
    const char *text,
    size_t length,
    const char *key,
    bool *out_value)
{
    const char *value = find_key(text, length, key);
    if (value == NULL || out_value == NULL) return UMI_STATUS_NOT_FOUND;
    if ((size_t)(value - text) + 4U <= length &&
        memcmp(value, "true", 4U) == 0) {
        *out_value = true;
        return UMI_STATUS_OK;
    }
    if ((size_t)(value - text) + 5U <= length &&
        memcmp(value, "false", 5U) == 0) {
        *out_value = false;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_PARSE_ERROR;
}

static UmiStatus object_extent(
    const char *start,
    const char *text,
    size_t length,
    const char **out_end)
{
    const char *cursor = start;
    const char *limit = text + length;
    int depth = 0;
    bool in_string = false;
    bool escaped = false;

    if (start == NULL || start >= limit || *start != '{') {
        return UMI_STATUS_PARSE_ERROR;
    }
    while (cursor < limit) {
        const char character = *cursor;
        if (in_string) {
            if (!escaped && character == '\\') escaped = true;
            else {
                if (!escaped && character == '\"') in_string = false;
                escaped = false;
            }
        } else if (character == '\"') {
            in_string = true;
        } else if (character == '{') {
            depth += 1;
        } else if (character == '}') {
            depth -= 1;
            if (depth == 0) {
                *out_end = cursor + 1;
                return UMI_STATUS_OK;
            }
        }
        cursor += 1;
    }
    return UMI_STATUS_PARSE_ERROR;
}

UmiStatus umi_workbench_layout_share_bundle_decode(
    const char *text,
    size_t length,
    UmiWorkbenchLayoutShareBundle *out_bundle)
{
    const char *manifest;
    const char *manifest_end;
    const char *document;
    const char *document_end;
    UmiWorkbenchLayoutJsonResult json_result;
    uint64_t number;
    UmiStatus status;

    if (text == NULL || out_bundle == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length == 0U) length = strlen(text);
    (void)memset(out_bundle, 0, sizeof(*out_bundle));
    out_bundle->structure_size = sizeof(*out_bundle);
    out_bundle->manifest.structure_size = sizeof(out_bundle->manifest);

    {
        char schema[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
        status = read_string_field(
            text, length, "schema", schema, sizeof(schema), true);
        if (status != UMI_STATUS_OK || strcmp(schema, UMI_SHARE_SCHEMA) != 0) {
            return UMI_STATUS_PARSE_ERROR;
        }
    }
    manifest = find_key(text, length, "manifest");
    if (manifest == NULL || *manifest != '{') return UMI_STATUS_PARSE_ERROR;
    status = object_extent(manifest, text, length, &manifest_end);
    if (status != UMI_STATUS_OK) return status;

    status = read_string_field(
        manifest,
        (size_t)(manifest_end - manifest),
        "bundle_id",
        out_bundle->manifest.bundle_id,
        sizeof(out_bundle->manifest.bundle_id),
        true);
    if (status == UMI_STATUS_OK) {
        status = read_string_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "layout_id",
            out_bundle->manifest.layout_id,
            sizeof(out_bundle->manifest.layout_id),
            true);
    }
    if (status == UMI_STATUS_OK) {
        status = read_string_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "layout_name",
            out_bundle->manifest.layout_name,
            sizeof(out_bundle->manifest.layout_name),
            true);
    }
    if (status == UMI_STATUS_OK) {
        status = read_string_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "source_owner_user_id",
            out_bundle->manifest.source_owner_user_id,
            sizeof(out_bundle->manifest.source_owner_user_id),
            false);
    }
    if (status == UMI_STATUS_OK) {
        status = read_string_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "source_owner_application_id",
            out_bundle->manifest.source_owner_application_id,
            sizeof(out_bundle->manifest.source_owner_application_id),
            false);
    }
    if (status == UMI_STATUS_OK) {
        status = read_string_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "source_workspace_id",
            out_bundle->manifest.source_workspace_id,
            sizeof(out_bundle->manifest.source_workspace_id),
            false);
    }
    if (status == UMI_STATUS_OK) {
        status = read_string_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "exported_by",
            out_bundle->manifest.exported_by,
            sizeof(out_bundle->manifest.exported_by),
            true);
    }
    if (status == UMI_STATUS_OK) {
        status = read_u64_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "exported_at_ms",
            &out_bundle->manifest.exported_at_ms);
    }
    if (status == UMI_STATUS_OK) {
        status = read_u64_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "layout_revision",
            &out_bundle->manifest.layout_revision);
    }
    if (status == UMI_STATUS_OK) {
        status = read_u64_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "content_hash",
            &out_bundle->manifest.content_hash);
    }
    if (status == UMI_STATUS_OK) {
        status = read_u64_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "schema_version",
            &number);
        if (status == UMI_STATUS_OK && number <= UINT32_MAX) {
            out_bundle->manifest.schema_version = (uint32_t)number;
        } else if (status == UMI_STATUS_OK) {
            status = UMI_STATUS_PARSE_ERROR;
        }
    }
    if (status == UMI_STATUS_OK) {
        status = read_bool_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "includes_runtime_geometry",
            &out_bundle->manifest.includes_runtime_geometry);
    }
    if (status == UMI_STATUS_OK) {
        status = read_bool_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "includes_audit",
            &out_bundle->manifest.includes_audit);
    }
    if (status != UMI_STATUS_OK) return status;

    status = read_bool_field(
        text,
        length,
        "signature_present",
        &out_bundle->signature_present);
    if (status != UMI_STATUS_OK) return status;
    status = read_string_field(
        text,
        length,
        "signature",
        out_bundle->signature,
        sizeof(out_bundle->signature),
        false);
    if (status != UMI_STATUS_OK) return status;

    document = find_key(text, length, "document");
    if (document == NULL || *document != '{') return UMI_STATUS_PARSE_ERROR;
    status = object_extent(document, text, length, &document_end);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_json_decode(
        document,
        (size_t)(document_end - document),
        &out_bundle->document,
        &json_result);
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_layout_share_bundle_validate(out_bundle);
}
