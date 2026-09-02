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

/* Provide the share write bytes operation used by this module and its client applications. */
static void share_write_bytes(
    UmiShareWriter *writer,
    const char *text,
    size_t length)
{
    size_t available;
    size_t copy_count;
    writer->required += length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (writer->buffer == NULL || writer->capacity == 0U) {
        writer->overflow = true;
        return;
    }
    available = writer->written < writer->capacity
        ? writer->capacity - writer->written
        : 0U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (available == 0U) {
        writer->overflow = true;
        return;
    }
    copy_count = length < available - 1U ? length : available - 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (copy_count > 0U) {
        (void)memcpy(writer->buffer + writer->written, text, copy_count);
        writer->written += copy_count;
    }
    writer->buffer[writer->written] = '\0';
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (copy_count != length) writer->overflow = true;
}

/*
 * Write share in its stable representation and report capacity or input failures to the
 * caller.
 */
static void share_write(UmiShareWriter *writer, const char *text)
{
    share_write_bytes(writer, text, strlen(text));
}

/* Provide the share format operation used by this module and its client applications. */
static void share_format(UmiShareWriter *writer, const char *format, ...)
{
    char temporary[512U];
    va_list arguments;
    int written;
    va_start(arguments, format);
    written = vsnprintf(temporary, sizeof(temporary), format, arguments);
    va_end(arguments);
    /* Apply this branch only when its contract condition is satisfied. */
    if (written < 0) {
        writer->overflow = true;
        return;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if ((size_t)written >= sizeof(temporary)) {
        writer->overflow = true;
        writer->required += (size_t)written;
        return;
    }
    share_write_bytes(writer, temporary, (size_t)written);
}

/*
 * Provide the escaped text allocate operation used by this module and its client
 * applications.
 */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_CAPACITY_EXCEEDED &&
        status != UMI_STATUS_OK) {
        return status;
    }
    escaped = (char *)malloc(required);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (escaped == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_workbench_layout_json_escape(
        text != NULL ? text : "", escaped, required, &required);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(escaped);
        return status;
    }
    *out_text = escaped;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_length != NULL) *out_length = strlen(escaped);
    return UMI_STATUS_OK;
}


/*
 * Provide the share write json string operation used by this module and its client
 * applications.
 */
static UmiStatus share_write_json_string(
    UmiShareWriter *writer,
    const char *text)
{
    char *escaped = NULL;
    size_t escaped_length = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (writer == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = escaped_text_allocate(
        text != NULL ? text : "",
        &escaped,
        &escaped_length);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    share_write_bytes(writer, escaped, escaped_length);
    free(escaped);
    return UMI_STATUS_OK;
}

/*
 * Provide the strip runtime geometry operation used by this module and its client
 * applications.
 */
static void strip_runtime_geometry(UmiWorkbenchLayoutDocument *document)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->node_count; ++index) {
        document->nodes[index].monitor_id[0] = '\0';
        (void)memset(
            &document->nodes[index].bounds,
            0,
            sizeof(document->nodes[index].bounds));
        document->nodes[index].z_order = 0;
    }
}

/* Provide the strip audit operation used by this module and its client applications. */
static void strip_audit(UmiWorkbenchLayoutDocument *document)
{
    (void)memset(&document->audit, 0, sizeof(document->audit));
}

/*
 * Provide the workbench layout import options default operation used by this module and
 * its client applications.
 */
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

/*
 * Initialise workbench layout share bundle from caller-provided values so later operations
 * receive a known state.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || out_bundle == NULL ||
        !umi_workbench_layout_text_present(bundle_id) ||
        !umi_workbench_layout_text_present(exported_by)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_document_validate_structure(document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_bundle, 0, sizeof(*out_bundle));
    out_bundle->structure_size = sizeof(*out_bundle);
    out_bundle->manifest.structure_size = sizeof(out_bundle->manifest);
    status = umi_workbench_layout_document_copy(
        &out_bundle->document, document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!include_runtime_geometry) strip_runtime_geometry(&out_bundle->document);
    /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Check that workbench layout share bundle satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_layout_share_bundle_validate(
    const UmiWorkbenchLayoutShareBundle *bundle)
{
    uint64_t calculated;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Use the stable identifier comparison to choose the matching record or policy. */
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
    /* Apply this branch only when its contract condition is satisfied. */
    if (calculated != bundle->manifest.content_hash ||
        calculated != bundle->document.content_hash) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout share bundle import operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_share_bundle_import(
    const UmiWorkbenchLayoutShareBundle *bundle,
    const UmiWorkbenchLayoutImportOptions *options,
    UmiWorkbenchLayoutDocument *out_document)
{
    UmiWorkbenchLayoutImportOptions effective;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bundle == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective = options != NULL
        ? *options
        : umi_workbench_layout_import_options_default();
    /* Apply this branch only when its contract condition is satisfied. */
    if (effective.structure_size < sizeof(effective)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (effective.require_integrity_match) {
        status = umi_workbench_layout_share_bundle_validate(bundle);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_workbench_layout_document_copy(
        out_document, &bundle->document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_workbench_layout_text_present(effective.target_layout_id)) {
        (void)umi_workbench_layout_copy_text(
            out_document->identity.layout_id,
            sizeof(out_document->identity.layout_id),
            effective.target_layout_id,
            false);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_workbench_layout_text_present(effective.target_owner_user_id)) {
        (void)umi_workbench_layout_copy_text(
            out_document->identity.owner_user_id,
            sizeof(out_document->identity.owner_user_id),
            effective.target_owner_user_id,
            false);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workbench_layout_text_present(
            effective.target_owner_application_id)) {
        (void)umi_workbench_layout_copy_text(
            out_document->identity.owner_application_id,
            sizeof(out_document->identity.owner_application_id),
            effective.target_owner_application_id,
            false);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_workbench_layout_text_present(effective.target_workspace_id)) {
        (void)umi_workbench_layout_copy_text(
            out_document->identity.workspace_id,
            sizeof(out_document->identity.workspace_id),
            effective.target_workspace_id,
            false);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!effective.preserve_source_audit) strip_audit(out_document);
    /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Write workbench layout share bundle in its stable representation and report capacity or
 * input failures to the caller.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bundle == NULL || out_required == NULL ||
        (buffer == NULL && capacity != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_share_bundle_validate(bundle);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_CAPACITY_EXCEEDED &&
        status != UMI_STATUS_OK) {
        return status;
    }
    document_capacity = json_result.bytes_required;
    document_json = (char *)malloc(document_capacity);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document_json == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_workbench_layout_json_encode(
        &bundle->document,
        &options,
        document_json,
        document_capacity,
        &json_result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(document_json);
        return status;
    }
    status = escaped_text_allocate(
        bundle->signature_present ? bundle->signature : "",
        &signature,
        &signature_length);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(document_json);
        return status;
    }

    (void)memset(&writer, 0, sizeof(writer));
    writer.buffer = buffer;
    writer.capacity = capacity;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer != NULL && capacity > 0U) buffer[0] = '\0';
    share_write(&writer, "{\n  \"schema\": ");
    status = share_write_json_string(&writer, UMI_SHARE_SCHEMA);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(signature);
        free(document_json);
        return status;
    }
    share_write(&writer, ",\n  \"manifest\": {\n");

    share_write(&writer, "    \"bundle_id\": ");
    status = share_write_json_string(&writer, bundle->manifest.bundle_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        share_write(&writer, ",\n    \"layout_id\": ");
        status = share_write_json_string(&writer, bundle->manifest.layout_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        share_write(&writer, ",\n    \"layout_name\": ");
        status = share_write_json_string(&writer, bundle->manifest.layout_name);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        share_write(&writer, ",\n    \"source_owner_user_id\": ");
        status = share_write_json_string(
            &writer, bundle->manifest.source_owner_user_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        share_write(
            &writer, ",\n    \"source_owner_application_id\": ");
        status = share_write_json_string(
            &writer, bundle->manifest.source_owner_application_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        share_write(&writer, ",\n    \"source_workspace_id\": ");
        status = share_write_json_string(
            &writer, bundle->manifest.source_workspace_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        share_write(&writer, ",\n    \"exported_by\": ");
        status = share_write_json_string(&writer, bundle->manifest.exported_by);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/* Provide the find key operation used by this module and its client applications. */
static const char *find_key(
    const char *text,
    size_t length,
    const char *key)
{
    char pattern[UMI_WORKBENCH_LAYOUT_ID_CAPACITY + 8U];
    const size_t pattern_length = strlen(key) + 2U;
    size_t index;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(key) + 3U >= sizeof(pattern)) return NULL;
    pattern[0] = '\"';
    (void)memcpy(pattern + 1U, key, strlen(key));
    pattern[strlen(key) + 1U] = '\"';
    pattern[strlen(key) + 2U] = '\0';
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index + pattern_length <= length; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (memcmp(text + index, pattern, pattern_length) == 0) {
            const char *cursor = text + index + pattern_length;
            const char *end = text + length;
            /*
             * Continue only while work remains available; the loop body advances the state on each
             * pass.
             */
            while (cursor < end &&
                   (*cursor == ' ' || *cursor == '\t' ||
                    *cursor == '\n' || *cursor == '\r')) {
                cursor += 1;
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (cursor < end && *cursor == ':') {
                cursor += 1;
                /*
                 * Continue only while work remains available; the loop body advances the state on each
                 * pass.
                 */
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

/* Provide the read string field operation used by this module and its client applications. */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (!required) {
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (buffer != NULL && capacity > 0U) buffer[0] = '\0';
            return UMI_STATUS_OK;
        }
        return UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (*value != '\"') return UMI_STATUS_PARSE_ERROR;
    cursor = value + 1;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while ((size_t)(cursor - text) < length) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (!escaped && *cursor == '\"') break;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!escaped && *cursor == '\\') escaped = true;
        /* Use this fallback path when the earlier condition does not apply. */
        else escaped = false;
        cursor += 1;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/* Provide the read u64 field operation used by this module and its client applications. */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || out_value == NULL) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (*value == '\"') value += 1;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while ((size_t)(value - text) < length && count + 1U < sizeof(token) &&
           *value >= '0' && *value <= '9') {
        token[count++] = *value++;
    }
    token[count] = '\0';
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count == 0U) return UMI_STATUS_PARSE_ERROR;
    end = NULL;
    *out_value = (uint64_t)strtoull(token, &end, 10);
    return end != token && *end == '\0'
        ? UMI_STATUS_OK
        : UMI_STATUS_PARSE_ERROR;
}

/* Provide the read bool field operation used by this module and its client applications. */
static UmiStatus read_bool_field(
    const char *text,
    size_t length,
    const char *key,
    bool *out_value)
{
    const char *value = find_key(text, length, key);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || out_value == NULL) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if ((size_t)(value - text) + 4U <= length &&
        memcmp(value, "true", 4U) == 0) {
        *out_value = true;
        return UMI_STATUS_OK;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if ((size_t)(value - text) + 5U <= length &&
        memcmp(value, "false", 5U) == 0) {
        *out_value = false;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_PARSE_ERROR;
}

/* Provide the object extent operation used by this module and its client applications. */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (start == NULL || start >= limit || *start != '{') {
        return UMI_STATUS_PARSE_ERROR;
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor < limit) {
        const char character = *cursor;
        /* Apply this branch only when its contract condition is satisfied. */
        if (in_string) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (!escaped && character == '\\') escaped = true;
            /* Use this fallback path when the earlier condition does not apply. */
            else {
                /* Apply this branch only when its contract condition is satisfied. */
                if (!escaped && character == '\"') in_string = false;
                escaped = false;
            }
        } else /* Apply this branch only when its contract condition is satisfied. */ if (character == '\"') {
            in_string = true;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (character == '{') {
            depth += 1;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (character == '}') {
            depth -= 1;
            /* Apply this branch only when its contract condition is satisfied. */
            if (depth == 0) {
                *out_end = cursor + 1;
                return UMI_STATUS_OK;
            }
        }
        cursor += 1;
    }
    return UMI_STATUS_PARSE_ERROR;
}

/*
 * Read workbench layout share bundle into validated module state and return a status when
 * input cannot be used.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_bundle == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length == 0U) length = strlen(text);
    (void)memset(out_bundle, 0, sizeof(*out_bundle));
    out_bundle->structure_size = sizeof(*out_bundle);
    out_bundle->manifest.structure_size = sizeof(out_bundle->manifest);

    {
        char schema[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
        status = read_string_field(
            text, length, "schema", schema, sizeof(schema), true);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK || strcmp(schema, UMI_SHARE_SCHEMA) != 0) {
            return UMI_STATUS_PARSE_ERROR;
        }
    }
    manifest = find_key(text, length, "manifest");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || *manifest != '{') return UMI_STATUS_PARSE_ERROR;
    status = object_extent(manifest, text, length, &manifest_end);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = read_string_field(
        manifest,
        (size_t)(manifest_end - manifest),
        "bundle_id",
        out_bundle->manifest.bundle_id,
        sizeof(out_bundle->manifest.bundle_id),
        true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = read_string_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "layout_id",
            out_bundle->manifest.layout_id,
            sizeof(out_bundle->manifest.layout_id),
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = read_string_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "layout_name",
            out_bundle->manifest.layout_name,
            sizeof(out_bundle->manifest.layout_name),
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = read_string_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "source_owner_user_id",
            out_bundle->manifest.source_owner_user_id,
            sizeof(out_bundle->manifest.source_owner_user_id),
            false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = read_string_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "source_owner_application_id",
            out_bundle->manifest.source_owner_application_id,
            sizeof(out_bundle->manifest.source_owner_application_id),
            false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = read_string_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "source_workspace_id",
            out_bundle->manifest.source_workspace_id,
            sizeof(out_bundle->manifest.source_workspace_id),
            false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = read_string_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "exported_by",
            out_bundle->manifest.exported_by,
            sizeof(out_bundle->manifest.exported_by),
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = read_u64_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "exported_at_ms",
            &out_bundle->manifest.exported_at_ms);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = read_u64_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "layout_revision",
            &out_bundle->manifest.layout_revision);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = read_u64_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "content_hash",
            &out_bundle->manifest.content_hash);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = read_u64_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "schema_version",
            &number);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (status == UMI_STATUS_OK && number <= UINT32_MAX) {
            out_bundle->manifest.schema_version = (uint32_t)number;
        } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) {
            status = UMI_STATUS_PARSE_ERROR;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = read_bool_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "includes_runtime_geometry",
            &out_bundle->manifest.includes_runtime_geometry);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = read_bool_field(
            manifest,
            (size_t)(manifest_end - manifest),
            "includes_audit",
            &out_bundle->manifest.includes_audit);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = read_bool_field(
        text,
        length,
        "signature_present",
        &out_bundle->signature_present);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = read_string_field(
        text,
        length,
        "signature",
        out_bundle->signature,
        sizeof(out_bundle->signature),
        false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    document = find_key(text, length, "document");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || *document != '{') return UMI_STATUS_PARSE_ERROR;
    status = object_extent(document, text, length, &document_end);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_json_decode(
        document,
        (size_t)(document_end - document),
        &out_bundle->document,
        &json_result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_layout_share_bundle_validate(out_bundle);
}
