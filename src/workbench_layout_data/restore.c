/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/restore.c
 *
 * PURPOSE:
 *   Implement transactional backup restore, record filtering and validation-only inspection for layout persistence data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/restore.h"
#include "umicom/workbench_layout_data/key_codec.h"
#include "umicom/workbench_layout_data/value_codec.h"
#include <stdlib.h>
#include <string.h>
#include "internal.h"




UmiWorkbenchLayoutRestoreOptions
umi_workbench_layout_restore_options_default(void)
{
    UmiWorkbenchLayoutRestoreOptions options;
    (void)memset(&options, 0, sizeof(options));
    options.structure_size = sizeof(options);
    options.replace_existing = true;
    options.restore_sessions = true;
    options.restore_outbox = true;
    options.restore_presence = false;
    options.validate_only = false;
    return options;
}

static bool restore_kind_allowed(
    UmiWorkbenchLayoutDataRecordKind kind,
    const UmiWorkbenchLayoutRestoreOptions *options)
{
    if ((kind ==
             UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_MANIFEST ||
         kind ==
             UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK) &&
        !options->restore_sessions) return false;
    if (kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_OUTBOX &&
        !options->restore_outbox) return false;
    if (kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_PRESENCE &&
        !options->restore_presence) return false;
    return true;
}

static UmiStatus restore_record(
    UmiDataServer *server,
    const char *line,
    size_t length,
    const UmiWorkbenchLayoutRestoreOptions *options,
    UmiWorkbenchLayoutRestoreResult *result)
{
    const char *separator;
    char *encoded_key;
    char *encoded_value;
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    UmiWorkbenchLayoutDataKeyParts parts;
    size_t key_length;
    size_t value_length;
    UmiStatus status;
    if (length < 8U || strncmp(line, "record=", 7U) != 0) {
        return UMI_STATUS_PARSE_ERROR;
    }
    separator = memchr(line + 7U, '|', length - 7U);
    if (separator == NULL) return UMI_STATUS_PARSE_ERROR;
    key_length = (size_t)(separator - (line + 7U));
    value_length = length - 7U - key_length - 1U;
    encoded_key = (char *)calloc(key_length + 1U, sizeof(char));
    encoded_value = (char *)calloc(value_length + 1U, sizeof(char));
    if (encoded_key == NULL || encoded_value == NULL) {
        free(encoded_key);
        free(encoded_value);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    (void)memcpy(encoded_key, line + 7U, key_length);
    (void)memcpy(encoded_value, separator + 1U, value_length);
    status = umi_workbench_layout_data_value_unescape(
        encoded_key, key, sizeof(key), NULL);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_value_unescape(
            encoded_value, value, sizeof(value), NULL);
    }
    free(encoded_key);
    free(encoded_value);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_data_key_parse(key, &parts);
    if (status != UMI_STATUS_OK) return status;
    result->parsed_count += 1U;
    if (!restore_kind_allowed(parts.kind, options)) {
        result->skipped_count += 1U;
        return UMI_STATUS_OK;
    }
    if (options->validate_only) return UMI_STATUS_OK;
    if (!options->replace_existing) {
        char existing[2];
        status = umi_data_server_get(server, key,
                                     existing, sizeof(existing));
        if (status == UMI_STATUS_OK ||
            status == UMI_STATUS_CAPACITY_EXCEEDED) {
            result->skipped_count += 1U;
            return UMI_STATUS_OK;
        }
        if (status != UMI_STATUS_NOT_FOUND) return status;
    }
    status = umi_data_server_set(server, key, value);
    if (status == UMI_STATUS_OK) {
        result->restored_count += 1U;
    }
    return status;
}

UmiStatus umi_workbench_layout_restore_apply(
    UmiDataServer *server,
    const char *backup,
    size_t length,
    const UmiWorkbenchLayoutRestoreOptions *options,
    UmiWorkbenchLayoutRestoreResult *out_result)
{
    UmiWorkbenchLayoutRestoreOptions effective;
    UmiWorkbenchLayoutRestoreResult result;
    UmiWorkbenchLayoutBackupManifest manifest;
    const char *cursor;
    bool transaction_started = false;
    UmiStatus status;
    if (server == NULL || backup == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective = options != NULL
        ? *options : umi_workbench_layout_restore_options_default();
    if (effective.structure_size < sizeof(effective)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&result, 0, sizeof(result));
    result.structure_size = sizeof(result);
    status = umi_workbench_layout_backup_validate(
        backup, length, &manifest);
    if (status == UMI_STATUS_OK && !effective.validate_only) {
        status = umi_workbench_layout_data_transaction_begin(
            server, &transaction_started);
    }
    cursor = strstr(backup, "records-begin\n");
    if (status == UMI_STATUS_OK && cursor == NULL) {
        status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK) {
        cursor += strlen("records-begin\n");
    }
    while (status == UMI_STATUS_OK &&
           strncmp(cursor, "records-end\n",
                   strlen("records-end\n")) != 0) {
        const char *end = strchr(cursor, '\n');
        if (end == NULL) {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        status = restore_record(
            server, cursor, (size_t)(end - cursor),
            &effective, &result);
        if (status != UMI_STATUS_OK) {
            result.failed_count += 1U;
            break;
        }
        cursor = end + 1;
    }
    status = umi_workbench_layout_data_transaction_finish(
        server, transaction_started, status);
    result.status = status;
    (void)umi_workbench_layout_data_copy_text(
        result.message, sizeof(result.message),
        status == UMI_STATUS_OK
            ? effective.validate_only
                ? "Layout backup validated without mutation."
                : "Layout backup restored transactionally."
            : "Layout backup restore failed and was rolled back.",
        true);
    if (out_result != NULL) *out_result = result;
    return status;
}
