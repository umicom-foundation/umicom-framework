/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/recovery.c
 *
 * PURPOSE:
 *   Implement atomic recovery records with explicit document identity,
 *   revision, source path and content length in a product-neutral store.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/recovery.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/atomic_file.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/threading.h"

#define UMI_RECOVERY_HEADER "UMICOM-RECOVERY-1\n"
#define UMI_RECOVERY_HEADER_CAPACITY 4096U

struct UmiRecoveryManager {
    char root[UMI_PATH_CAPACITY];
    UmiMutex *mutex;
};

/* Provide the recovery path operation used by this module and its client applications. */
static UmiStatus umi_recovery_path(const UmiRecoveryManager *manager,
                                   UmiDocumentId document_id,
                                   char *out_path,
                                   size_t capacity)
{
    char name[96];
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL || document_id == 0U ||
        out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(name,
                       sizeof(name),
                       "%" PRIu64 ".recovery",
                       document_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(name)) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    return umi_fs_join(out_path, capacity, manager->root, name);
}

/*
 * Initialise recovery manager from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_recovery_manager_create(const char *root_directory,
                                      UmiRecoveryManager **out_manager)
{
    UmiRecoveryManager *manager;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root_directory == NULL || root_directory[0] == '\0' ||
        out_manager == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_manager = NULL;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(root_directory) >= UMI_PATH_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    manager = (UmiRecoveryManager *)calloc(1U, sizeof(*manager));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    (void)snprintf(manager->root,
                   sizeof(manager->root),
                   "%s",
                   root_directory);
    (void)umi_fs_normalise(manager->root);

    status = umi_fs_make_directories(manager->root);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(manager);
        return status;
    }
    status = umi_mutex_create(&manager->mutex);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(manager);
        return status;
    }

    *out_manager = manager;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by recovery manager so the same storage can be reused
 * safely.
 */
void umi_recovery_manager_destroy(UmiRecoveryManager *manager)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL) {
        return;
    }
    umi_mutex_destroy(manager->mutex);
    free(manager);
}

/*
 * Write recovery manager in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_recovery_manager_save(UmiRecoveryManager *manager,
                                    UmiDocumentId document_id,
                                    const char *source_path,
                                    uint64_t revision,
                                    const char *text,
                                    size_t length)
{
    char path[UMI_PATH_CAPACITY];
    char header[UMI_RECOVERY_HEADER_CAPACITY];
    size_t source_length;
    size_t header_length;
    size_t total_length;
    char *payload;
    int written;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL || document_id == 0U ||
        source_path == NULL || (text == NULL && length > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strchr(source_path, '\n') != NULL ||
        strchr(source_path, '\r') != NULL ||
        strlen(source_path) >= UMI_PATH_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_recovery_path(manager, document_id, path, sizeof(path));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    source_length = strlen(source_path);
    written = snprintf(header,
                       sizeof(header),
                       UMI_RECOVERY_HEADER
                       "document=%" PRIu64 "\n"
                       "revision=%" PRIu64 "\n"
                       "source_length=%zu\n"
                       "content_length=%zu\n\n",
                       document_id,
                       revision,
                       source_length,
                       length);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(header)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    header_length = (size_t)written;

    /* Apply this branch only when its contract condition is satisfied. */
    if (header_length > SIZE_MAX - source_length ||
        header_length + source_length > SIZE_MAX - length) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    total_length = header_length + source_length + length;
    payload = (char *)malloc(total_length > 0U ? total_length : 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (payload == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    (void)memcpy(payload, header, header_length);
    (void)memcpy(payload + header_length, source_path, source_length);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) {
        (void)memcpy(payload + header_length + source_length, text, length);
    }

    (void)umi_mutex_lock(manager->mutex);
    status = umi_atomic_file_write(path, payload, total_length);
    (void)umi_mutex_unlock(manager->mutex);
    free(payload);
    return status;
}

/*
 * Provide the recovery parse number operation used by this module and its client
 * applications.
 */
static UmiStatus umi_recovery_parse_number(const char *line,
                                           const char *prefix,
                                           uint64_t *out_value)
{
    char *end = NULL;
    unsigned long long value;
    size_t prefix_length = strlen(prefix);

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(line, prefix, prefix_length) != 0) {
        return UMI_STATUS_PARSE_ERROR;
    }
    value = strtoull(line + prefix_length, &end, 10);
    /* Apply this branch only when its contract condition is satisfied. */
    if (end == line + prefix_length || *end != '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }
    *out_value = (uint64_t)value;
    return UMI_STATUS_OK;
}

/* Return the number of records represented by recovery parse without changing their state. */
static UmiStatus umi_recovery_parse_size(const char *line,
                                         const char *prefix,
                                         size_t *out_value)
{
    uint64_t value;
    UmiStatus status = umi_recovery_parse_number(line, prefix, &value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || value > SIZE_MAX) {
        return status != UMI_STATUS_OK
            ? status
            : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    *out_value = (size_t)value;
    return UMI_STATUS_OK;
}

/*
 * Read recovery manager into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_recovery_manager_load(const UmiRecoveryManager *manager,
                                    UmiDocumentId document_id,
                                    UmiRecoveryRecord *out_record)
{
    char path[UMI_PATH_CAPACITY];
    char *payload = NULL;
    size_t payload_length = 0U;
    char *cursor;
    char *line_end;
    uint64_t parsed_document = 0U;
    uint64_t revision = 0U;
    size_t source_length = 0U;
    size_t content_length = 0U;
    size_t header_length;
    UmiRecoveryManager *mutable_manager;
    UmiStatus status;
    int line_number;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL || document_id == 0U || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_record, 0, sizeof(*out_record));

    status = umi_recovery_path(manager, document_id, path, sizeof(path));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    mutable_manager = (UmiRecoveryManager *)manager;
    (void)umi_mutex_lock(mutable_manager->mutex);
    status = umi_fs_read_text(path, &payload, &payload_length);
    (void)umi_mutex_unlock(mutable_manager->mutex);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    cursor = payload;
    /* Visit each bounded item once so every record receives the same rule. */
    for (line_number = 0; line_number < 6; ++line_number) {
        line_end = strchr(cursor, '\n');
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (line_end == NULL) {
            umi_fs_free_text(payload);
            return UMI_STATUS_PARSE_ERROR;
        }
        *line_end = '\0';

        /* Select the behaviour associated with the requested command or state value. */
        switch (line_number) {
            case 0:
                /* Use the stable identifier comparison to choose the matching record or policy. */
                if (strcmp(cursor, "UMICOM-RECOVERY-1") != 0) {
                    umi_fs_free_text(payload);
                    return UMI_STATUS_PARSE_ERROR;
                }
                break;
            case 1:
                status = umi_recovery_parse_number(cursor,
                                                   "document=",
                                                   &parsed_document);
                break;
            case 2:
                status = umi_recovery_parse_number(cursor,
                                                   "revision=",
                                                   &revision);
                break;
            case 3:
                status = umi_recovery_parse_size(cursor,
                                                 "source_length=",
                                                 &source_length);
                break;
            case 4:
                status = umi_recovery_parse_size(cursor,
                                                 "content_length=",
                                                 &content_length);
                break;
            case 5:
                status = cursor[0] == '\0'
                    ? UMI_STATUS_OK
                    : UMI_STATUS_PARSE_ERROR;
                break;
            default:
                status = UMI_STATUS_INTERNAL_ERROR;
                break;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            umi_fs_free_text(payload);
            return status;
        }
        cursor = line_end + 1;
    }

    header_length = (size_t)(cursor - payload);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (parsed_document != document_id ||
        source_length >= sizeof(out_record->source_path) ||
        header_length > payload_length ||
        source_length > payload_length - header_length ||
        content_length > payload_length - header_length - source_length ||
        header_length + source_length + content_length != payload_length) {
        umi_fs_free_text(payload);
        return UMI_STATUS_PARSE_ERROR;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (source_length > 0U) {
        (void)memcpy(out_record->source_path, cursor, source_length);
    }
    out_record->source_path[source_length] = '\0';
    out_record->text = (char *)malloc(content_length + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_record->text == NULL) {
        umi_fs_free_text(payload);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (content_length > 0U) {
        (void)memcpy(out_record->text,
                     cursor + source_length,
                     content_length);
    }
    out_record->text[content_length] = '\0';
    out_record->length = content_length;
    out_record->document_id = parsed_document;
    out_record->revision = revision;

    umi_fs_free_text(payload);
    return UMI_STATUS_OK;
}

/* Release or reset state held by recovery record so the same storage can be reused safely. */
void umi_recovery_record_dispose(UmiRecoveryRecord *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) {
        return;
    }
    free(record->text);
    (void)memset(record, 0, sizeof(*record));
}

/*
 * Provide the recovery manager exists operation used by this module and its client
 * applications.
 */
int umi_recovery_manager_exists(const UmiRecoveryManager *manager,
                                UmiDocumentId document_id)
{
    char path[UMI_PATH_CAPACITY];
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_recovery_path(manager, document_id, path, sizeof(path)) !=
        UMI_STATUS_OK) {
        return 0;
    }
    return umi_fs_is_file(path);
}

/*
 * Remove recovery manager while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_recovery_manager_remove(UmiRecoveryManager *manager,
                                      UmiDocumentId document_id)
{
    char path[UMI_PATH_CAPACITY];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL || document_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_recovery_path(manager, document_id, path, sizeof(path));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    (void)umi_mutex_lock(manager->mutex);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_fs_exists(path)) {
        status = UMI_STATUS_OK;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        status = remove(path) == 0
            ? UMI_STATUS_OK
            : UMI_STATUS_IO_ERROR;
    }
    (void)umi_mutex_unlock(manager->mutex);
    return status;
}

/*
 * Provide the recovery manager purge operation used by this module and its client
 * applications.
 */
UmiStatus umi_recovery_manager_purge(UmiRecoveryManager *manager)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manager == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)umi_mutex_lock(manager->mutex);
    status = umi_fs_remove_tree(manager->root);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_fs_make_directories(manager->root);
    }
    (void)umi_mutex_unlock(manager->mutex);
    return status;
}

/*
 * Provide the recovery manager root operation used by this module and its client
 * applications.
 */
const char *umi_recovery_manager_root(const UmiRecoveryManager *manager)
{
    return manager != NULL ? manager->root : "";
}
