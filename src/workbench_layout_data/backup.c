/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/backup.c
 *
 * PURPOSE:
 *   Implement deterministic portable backups of Data Server-owned layout records with policy filtering and integrity evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/backup.h"
#include "umicom/workbench_layout_data/key_codec.h"
#include "umicom/workbench_layout_data/value_codec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct BackupWriter {
    char *buffer;
    size_t capacity;
    size_t written;
    size_t required;
    bool overflow;
    UmiWorkbenchLayoutBackupOptions options;
    UmiWorkbenchLayoutBackupManifest manifest;
} BackupWriter;

/* Provide the append text operation used by this module and its client applications. */
static void append_text(BackupWriter *writer, const char *text)
{
    const size_t length = strlen(text);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (writer->buffer != NULL &&
        writer->written + length < writer->capacity) {
        (void)memcpy(writer->buffer + writer->written, text, length);
    } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (writer->buffer != NULL) {
        writer->overflow = true;
    }
    writer->written += length;
    writer->required += length;
}

/* Provide the kind included operation used by this module and its client applications. */
static bool kind_included(
    const BackupWriter *writer,
    UmiWorkbenchLayoutDataRecordKind kind)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if ((kind ==
             UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_MANIFEST ||
         kind ==
             UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK) &&
        !writer->options.include_sessions) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_OUTBOX &&
        !writer->options.include_outbox) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_PRESENCE &&
        !writer->options.include_presence) {
        return false;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_MIGRATION &&
        !writer->options.include_migrations) {
        return false;
    }
    return true;
}

/* Provide the backup accept operation used by this module and its client applications. */
static UmiStatus backup_accept(
    const char *key,
    const char *value,
    void *context)
{
    BackupWriter *writer = (BackupWriter *)context;
    UmiWorkbenchLayoutDataKeyParts parts;
    char *escaped_key = NULL;
    char *escaped_value = NULL;
    size_t key_required = 0U;
    size_t value_required = 0U;
    UmiStatus status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_workbench_layout_data_key_is_owned(key)) {
        return UMI_STATUS_OK;
    }
    status = umi_workbench_layout_data_key_parse(key, &parts);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!kind_included(writer, parts.kind)) return UMI_STATUS_OK;
    status = umi_workbench_layout_data_value_escape(
        key, NULL, 0U, &key_required);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_value_escape(
            value, NULL, 0U, &value_required);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    escaped_key = (char *)calloc(key_required, sizeof(char));
    escaped_value = (char *)calloc(value_required, sizeof(char));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (escaped_key == NULL || escaped_value == NULL) {
        free(escaped_key);
        free(escaped_value);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    status = umi_workbench_layout_data_value_escape(
        key, escaped_key, key_required, NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_value_escape(
            value, escaped_value, value_required, NULL);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        append_text(writer, "record=");
        append_text(writer, escaped_key);
        append_text(writer, "|");
        append_text(writer, escaped_value);
        append_text(writer, "\n");
        writer->manifest.record_count += 1U;
        writer->manifest.byte_count +=
            strlen(key) + strlen(value);
        /* Apply this branch only when its contract condition is satisfied. */
        if (parts.sequence != 0U) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (writer->manifest.first_sequence == 0U ||
                parts.sequence < writer->manifest.first_sequence) {
                writer->manifest.first_sequence = parts.sequence;
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (parts.sequence > writer->manifest.last_sequence) {
                writer->manifest.last_sequence = parts.sequence;
            }
        }
    }
    free(escaped_key);
    free(escaped_value);
    return status;
}

/*
 * Provide the workbench layout backup options default operation used by this module and
 * its client applications.
 */
UmiWorkbenchLayoutBackupOptions
umi_workbench_layout_backup_options_default(void)
{
    UmiWorkbenchLayoutBackupOptions options;
    (void)memset(&options, 0, sizeof(options));
    options.structure_size = sizeof(options);
    options.include_sessions = true;
    options.include_audit = true;
    options.include_outbox = true;
    options.include_presence = false;
    options.include_migrations = true;
    return options;
}

/* Provide the write header operation used by this module and its client applications. */
static void write_header(BackupWriter *writer)
{
    char number[64];
    append_text(writer, "UMICOM-WORKBENCH-LAYOUT-BACKUP\n");
    append_text(writer, "format=umicom.workbench-layout-backup/1\n");
    append_text(writer, "backup_id=");
    append_text(writer, writer->manifest.backup_id);
    append_text(writer, "\n");
    append_text(writer, "source_replica_id=");
    append_text(writer, writer->manifest.source_replica_id);
    append_text(writer, "\n");
    (void)snprintf(number, sizeof(number), "%u",
                   writer->manifest.schema_version);
    append_text(writer, "schema_version=");
    append_text(writer, number);
    append_text(writer, "\n");
    (void)snprintf(number, sizeof(number), "%llu",
                   (unsigned long long)writer->manifest.created_at_ms);
    append_text(writer, "created_at_ms=");
    append_text(writer, number);
    append_text(writer, "\n");
    append_text(writer, "records-begin\n");
}

/* Provide the write footer operation used by this module and its client applications. */
static void write_footer(BackupWriter *writer)
{
    char hash_text[32];
    append_text(writer, "records-end\n");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (writer->buffer != NULL && !writer->overflow) {
        writer->manifest.content_hash =
            umi_workbench_layout_data_hash_bytes(
                writer->buffer, writer->written);
    }
    (void)snprintf(hash_text, sizeof(hash_text),
                   "%016llx",
                   (unsigned long long)writer->manifest.content_hash);
    append_text(writer, "content_hash=");
    append_text(writer, hash_text);
    append_text(writer, "\n");
}

/*
 * Initialise workbench layout backup from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_workbench_layout_backup_create(
    const UmiDataServer *server,
    const char *backup_id,
    const char *replica_id,
    uint64_t created_at_ms,
    const UmiWorkbenchLayoutBackupOptions *options,
    char *buffer,
    size_t capacity,
    UmiWorkbenchLayoutBackupResult *out_result)
{
    BackupWriter writer;
    UmiWorkbenchLayoutBackupOptions effective;
    UmiWorkbenchLayoutBackupResult result;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL || backup_id == NULL ||
        replica_id == NULL || (buffer == NULL && capacity != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective = options != NULL
        ? *options : umi_workbench_layout_backup_options_default();
    /* Apply this branch only when its contract condition is satisfied. */
    if (effective.structure_size < sizeof(effective)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&writer, 0, sizeof(writer));
    writer.buffer = buffer;
    writer.capacity = capacity;
    writer.options = effective;
    writer.manifest.structure_size = sizeof(writer.manifest);
    writer.manifest.schema_version =
        UMI_WORKBENCH_LAYOUT_DATA_SCHEMA_VERSION;
    writer.manifest.created_at_ms = created_at_ms;
    writer.manifest.includes_sessions = effective.include_sessions;
    writer.manifest.includes_audit = effective.include_audit;
    writer.manifest.includes_outbox = effective.include_outbox;
    (void)umi_workbench_layout_data_copy_text(
        writer.manifest.backup_id,
        sizeof(writer.manifest.backup_id),
        backup_id, false);
    (void)umi_workbench_layout_data_copy_text(
        writer.manifest.source_replica_id,
        sizeof(writer.manifest.source_replica_id),
        replica_id, false);
    write_header(&writer);
    status = umi_data_server_visit(server, backup_accept, &writer);
    write_footer(&writer);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer != NULL && writer.written < capacity) {
        buffer[writer.written] = '\0';
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        (writer.overflow || buffer == NULL)) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memset(&result, 0, sizeof(result));
    result.structure_size = sizeof(result);
    result.status = status;
    result.manifest = writer.manifest;
    result.bytes_required = writer.required + 1U;
    result.bytes_written =
        buffer != NULL && !writer.overflow ? writer.written : 0U;
    (void)umi_workbench_layout_data_copy_text(
        result.message, sizeof(result.message),
        status == UMI_STATUS_OK
            ? "Layout backup created."
            : status == UMI_STATUS_CAPACITY_EXCEEDED
                ? "Backup buffer is too small."
                : "Layout backup failed.",
        true);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_result != NULL) *out_result = result;
    return status;
}

/* Provide the header value operation used by this module and its client applications. */
static UmiStatus header_value(
    const char *backup,
    const char *name,
    char *buffer,
    size_t capacity)
{
    char pattern[128];
    const char *start;
    const char *end;
    size_t length;
    (void)snprintf(pattern, sizeof(pattern), "%s=", name);
    start = strstr(backup, pattern);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (start == NULL) return UMI_STATUS_NOT_FOUND;
    start += strlen(pattern);
    end = strchr(start, '\n');
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (end == NULL) return UMI_STATUS_PARSE_ERROR;
    length = (size_t)(end - start);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(buffer, start, length);
    buffer[length] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Check that workbench layout backup satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_workbench_layout_backup_validate(
    const char *backup,
    size_t length,
    UmiWorkbenchLayoutBackupManifest *out_manifest)
{
    UmiWorkbenchLayoutBackupManifest manifest;
    char value[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    unsigned int version;
    unsigned long long timestamp;
    unsigned long long expected_hash;
    uint64_t actual_hash;
    const char *cursor;
    const char *records_end;
    const char *hash_line;
    const char *hash_end;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (backup == NULL || out_manifest == NULL ||
        length != strlen(backup)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(backup,
                "UMICOM-WORKBENCH-LAYOUT-BACKUP\n"
                "format=umicom.workbench-layout-backup/1\n",
                strlen("UMICOM-WORKBENCH-LAYOUT-BACKUP\n"
                       "format=umicom.workbench-layout-backup/1\n")) != 0 ||
        strstr(backup, "records-begin\n") == NULL) {
        return UMI_STATUS_PARSE_ERROR;
    }
    records_end = strstr(backup, "records-end\n");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (records_end == NULL) return UMI_STATUS_PARSE_ERROR;
    hash_line = records_end + strlen("records-end\n");
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(hash_line, "content_hash=",
                strlen("content_hash=")) != 0) {
        return UMI_STATUS_PARSE_ERROR;
    }
    hash_end = strchr(hash_line, '\n');
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hash_end == NULL || hash_end + 1 != backup + length ||
        sscanf(hash_line + strlen("content_hash="),
               "%16llx", &expected_hash) != 1) {
        return UMI_STATUS_PARSE_ERROR;
    }
    actual_hash = umi_workbench_layout_data_hash_bytes(
        backup, (size_t)(hash_line - backup));
    /* Apply this branch only when its contract condition is satisfied. */
    if (actual_hash != (uint64_t)expected_hash) {
        return UMI_STATUS_PARSE_ERROR;
    }
    (void)memset(&manifest, 0, sizeof(manifest));
    manifest.structure_size = sizeof(manifest);
    /* Apply this branch only when its contract condition is satisfied. */
    if (header_value(backup, "backup_id", value, sizeof(value)) !=
        UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    (void)umi_workbench_layout_data_copy_text(
        manifest.backup_id, sizeof(manifest.backup_id),
        value, false);
    /* Apply this branch only when its contract condition is satisfied. */
    if (header_value(backup, "source_replica_id",
                     value, sizeof(value)) != UMI_STATUS_OK) {
        return UMI_STATUS_PARSE_ERROR;
    }
    (void)umi_workbench_layout_data_copy_text(
        manifest.source_replica_id,
        sizeof(manifest.source_replica_id),
        value, false);
    /* Apply this branch only when its contract condition is satisfied. */
    if (header_value(backup, "schema_version",
                     value, sizeof(value)) != UMI_STATUS_OK ||
        sscanf(value, "%u", &version) != 1) {
        return UMI_STATUS_PARSE_ERROR;
    }
    manifest.schema_version = (uint32_t)version;
    /* Apply this branch only when its contract condition is satisfied. */
    if (header_value(backup, "created_at_ms",
                     value, sizeof(value)) != UMI_STATUS_OK ||
        sscanf(value, "%llu", &timestamp) != 1) {
        return UMI_STATUS_PARSE_ERROR;
    }
    manifest.created_at_ms = (uint64_t)timestamp;
    cursor = strstr(backup, "records-begin\n") +
        strlen("records-begin\n");
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor < backup + length &&
           strncmp(cursor, "records-end\n",
                   strlen("records-end\n")) != 0) {
        const char *end = strchr(cursor, '\n');
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL || strncmp(cursor, "record=", 7U) != 0 ||
            memchr(cursor, '|', (size_t)(end - cursor)) == NULL) {
            return UMI_STATUS_PARSE_ERROR;
        }
        count += 1U;
        cursor = end + 1;
    }
    manifest.record_count = count;
    manifest.content_hash = (uint64_t)expected_hash;
    *out_manifest = manifest;
    return UMI_STATUS_OK;
}
