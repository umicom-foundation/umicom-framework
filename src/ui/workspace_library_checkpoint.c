/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workspace_library_checkpoint.c
 * PURPOSE: Save and recover the complete ordered named-layout list using
 * framed existing layout codec records and transactional Data Server chunks.
 * AUTHOR: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/workspace_library_checkpoint.h"
#include "umicom/ui/workspace_library.h"
#include "umicom/document/text_encoding.h"
#include "umicom/workbench_layout_data/chunk_store.h"
#include "workspace_checkpoint_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIBRARY_MAGIC "UMICOM-WORKSPACE-LIBRARY 1\n"
#define LIBRARY_CONTEXT_RESERVE \
    (UMI_UI_WINDOW_GROUP_MAX * UMI_UI_WINDOW_GROUP_MAX_MEMBERS + \
     2U * UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS + 8U)

/* Protect both the immutable validation owner and the transactional output. */
static bool storage_overlaps(const void *left, size_t left_size,
                             const void *right, size_t right_size)
{
    const uintptr_t a = (uintptr_t)left, b = (uintptr_t)right;
    if (left == NULL || right == NULL) return false;
    return a <= b ? b - a < left_size : a - b < right_size;
}

/* Keep early errors distinguishable from corrupt metadata discovered on disk. */
static void library_report_init(UmiDataServer *server,
                                UmiUiWorkspaceLibraryCheckpointReport *report)
{
    memset(report, 0, sizeof(*report));
    umi_ui_checkpoint_internal_report_init(server, &report->checkpoint);
}

/* Reuse copied-library and checkpoint guards before any codec/string lookup. */
static UmiStatus library_validate_host(
    const UmiUiWorkspaceCustomisation *model,
    const UmiUiWorkspaceCheckpointScope *scope)
{
    UmiUiWorkspaceLibrarySnapshot *snapshot;
    UmiUiWorkspaceLibraryPolicy policy;
    UmiStatus status = umi_ui_checkpoint_internal_validate_host(model);
    size_t index;
    if (status != UMI_STATUS_OK) return status;
    policy.layout_prefix = scope->layout_prefix;
    snapshot = malloc(sizeof(*snapshot));
    if (snapshot == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_ui_workspace_library_snapshot(model, &policy, snapshot);
    free(snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (model->groups.revision > UINT64_MAX - LIBRARY_CONTEXT_RESERVE)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (index = 0U; index < model->groups.count; ++index) {
        const UmiUiWindowGroup *group = &model->groups.items[index];
        size_t member;
        if (group->revision > UINT64_MAX - LIBRARY_CONTEXT_RESERVE)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        for (member = 0U; member < group->member_count; ++member)
            if (group->members[member].role < UMI_UI_WINDOW_GROUP_SOURCE ||
                group->members[member].role > UMI_UI_WINDOW_GROUP_BIDIRECTIONAL)
                return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* Append one exact byte span while retaining room for a terminating NUL. */
static UmiStatus archive_append(char *archive, size_t *used,
                                const char *text, size_t length)
{
    if (length >= UMI_UI_WORKSPACE_LIBRARY_ARCHIVE_CAPACITY - *used)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(archive + *used, text, length);
    *used += length;
    archive[*used] = '\0';
    return UMI_STATUS_OK;
}

/* Decimal line lengths delimit codec payloads, never their internal grammar. */
static UmiStatus archive_append_number(char *archive, size_t *used, size_t number)
{
    char line[32U];
    const int written = snprintf(line, sizeof(line), "%zu\n", number);
    if (written < 0 || (size_t)written >= sizeof(line)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return archive_append(archive, used, line, (size_t)written);
}

/* Encode every committed record before opening the storage transaction. */
static UmiStatus archive_encode(const UmiUiWorkspaceCheckpointScope *scope,
                                const UmiUiWorkspaceCustomisation *model,
                                uint64_t saved_at_ns, char **out_archive)
{
    UmiUiLayoutPersistenceRecord *record = NULL;
    char *archive = NULL, *text = NULL;
    size_t used = 0U, index;
    UmiStatus status;
    if (model->layout_count == 0U) return UMI_STATUS_NOT_FOUND;
    archive = malloc(UMI_UI_WORKSPACE_LIBRARY_ARCHIVE_CAPACITY);
    text = malloc(UMI_UI_LAYOUT_ENCODED_CAPACITY);
    record = calloc(1U, sizeof(*record));
    if (archive == NULL || text == NULL || record == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY; goto done;
    }
    archive[0] = '\0';
    status = archive_append(archive, &used, LIBRARY_MAGIC, sizeof(LIBRARY_MAGIC) - 1U);
    if (status == UMI_STATUS_OK) status = archive_append_number(archive, &used, model->layout_count);
    if (status == UMI_STATUS_OK) status = archive_append_number(archive, &used, strlen(model->active_layout_id));
    if (status == UMI_STATUS_OK) status = archive_append(archive, &used,
        model->active_layout_id, strlen(model->active_layout_id));
    if (status == UMI_STATUS_OK) status = archive_append(archive, &used, "\n", 1U);
    for (index = 0U; status == UMI_STATUS_OK && index < model->layout_count; ++index) {
        record->schema_version = UMI_UI_LAYOUT_PERSISTENCE_SCHEMA_VERSION;
        record->saved_at_ns = saved_at_ns;
        record->layout = model->layouts[index];
        status = umi_ui_checkpoint_internal_validate_record(scope, model, record);
        if (status == UMI_STATUS_OK)
            status = umi_ui_layout_persistence_encode(record, text, UMI_UI_LAYOUT_ENCODED_CAPACITY);
        if (status == UMI_STATUS_OK) status = archive_append_number(archive, &used, strlen(text));
        if (status == UMI_STATUS_OK) status = archive_append(archive, &used, text, strlen(text));
        if (status == UMI_STATUS_OK) status = archive_append(archive, &used, "\n", 1U);
    }
    if (status == UMI_STATUS_OK) {
        *out_archive = archive;
        archive = NULL;
    }
done:
    free(record); free(text); free(archive);
    return status;
}

/* Parse a bounded canonical unsigned decimal line without arithmetic wrap. */
static UmiStatus archive_read_number(const char **cursor, const char *end,
                                     size_t maximum, size_t *out_number)
{
    const char *start = *cursor;
    size_t number = 0U, digits = 0U;
    while (*cursor < end && **cursor != '\n') {
        const unsigned char value = (unsigned char)**cursor;
        size_t digit;
        if (value < '0' || value > '9' || digits >= 20U) return UMI_STATUS_PARSE_ERROR;
        digit = (size_t)(value - '0');
        if (digit > maximum || number > (maximum - digit) / 10U) return UMI_STATUS_PARSE_ERROR;
        number = number * 10U + digit;
        ++*cursor;
        ++digits;
    }
    if (*cursor == end || digits == 0U || (digits > 1U && start[0] == '0'))
        return UMI_STATUS_PARSE_ERROR;
    ++*cursor;
    *out_number = number;
    return UMI_STATUS_OK;
}

/* Reject cross-layout aliasing of a single stable window to different tools. */
static UmiStatus archive_check_previous(const UmiUiWorkspaceCustomisation *candidate,
                                       const UmiUiWorkspaceLayout *layout)
{
    size_t saved, index;
    for (saved = 0U; saved < candidate->layout_count; ++saved) {
        const UmiUiWorkspaceLayout *previous = &candidate->layouts[saved];
        if (strcmp(previous->layout_id, layout->layout_id) == 0) return UMI_STATUS_PARSE_ERROR;
        for (index = 0U; index < layout->window_count; ++index) {
            const UmiUiWorkspaceWindow *window = &layout->windows[index];
            const UmiUiWorkspaceWindow *existing =
                umi_ui_workspace_layout_find_window(previous, window->window_id);
            if (existing != NULL && strcmp(existing->tool_id, window->tool_id) != 0)
                return UMI_STATUS_INVALID_STATE;
        }
    }
    return UMI_STATUS_OK;
}

/* Remove only old/new active-layout links; unrelated host members survive. */
static UmiStatus clear_layout_contexts(UmiUiWindowGroupStore *groups,
                                       const UmiUiWorkspaceLayout *layout)
{
    size_t index;
    if (layout == NULL) return UMI_STATUS_OK;
    for (index = 0U; index < layout->window_count; ++index) {
        UmiStatus status = umi_ui_window_group_unassign(groups, layout->windows[index].window_id);
        if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) return status;
    }
    return UMI_STATUS_OK;
}

/* Stage one possible active routing projection independently. Resetting from
 * the host also lets inactive layouts prove context capacity without leaking
 * their links into the final selected layout's routing graph. */
static UmiStatus archive_project_contexts(const UmiUiWorkspaceCustomisation *host,
                                          UmiUiWorkspaceCustomisation *candidate,
                                          const UmiUiWorkspaceLayout *active)
{
    const UmiUiWorkspaceLayout *old_active = umi_ui_workspace_customisation_active_const(host);
    UmiStatus status;
    size_t index;
    candidate->groups = host->groups;
    status = clear_layout_contexts(&candidate->groups, old_active);
    if (status == UMI_STATUS_OK) status = clear_layout_contexts(&candidate->groups, active);
    if (status != UMI_STATUS_OK || active == NULL) return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_STATE;
    for (index = 0U; index < active->window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &active->windows[index];
        UmiUiWindowGroupRole role = UMI_UI_WINDOW_GROUP_BIDIRECTIONAL;
        const UmiUiWindowGroup *previous;
        if (window->context_group_id[0] == '\0') continue;
        previous = umi_ui_window_group_for_window(&host->groups, window->window_id, &role);
        if (previous == NULL || strcmp(previous->group_id, window->context_group_id) != 0)
            role = UMI_UI_WINDOW_GROUP_BIDIRECTIONAL;
        status = umi_ui_window_group_assign(&candidate->groups,
            window->context_group_id, window->window_id, role);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/* Decode an ordered archive into private candidate storage. Delimiter/count
 * errors, repeated IDs, bad hidden records and trailing bytes all fail closed. */
static UmiStatus archive_decode(const UmiUiWorkspaceCheckpointScope *scope,
                                const UmiUiWorkspaceCustomisation *host,
                                const char *archive,
                                UmiUiWorkspaceCustomisation *candidate,
                                UmiUiWorkspaceLibraryCheckpointReport *report)
{
    UmiUiLayoutPersistenceRecord *record = NULL;
    UmiUiWorkspaceLibrarySnapshot *snapshot = NULL;
    UmiUiWorkspaceLibraryPolicy policy = {scope->layout_prefix};
    char *text = NULL;
    const size_t length = strlen(archive);
    const char *cursor = archive, *end = archive + length;
    size_t count, active_length, index, active_matches = 0U;
    uint64_t saved_at_ns = 0U, maximum_revision = 0U;
    UmiStatus status;
    if (length >= UMI_UI_WORKSPACE_LIBRARY_ARCHIVE_CAPACITY ||
        length < sizeof(LIBRARY_MAGIC) - 1U ||
        memcmp(cursor, LIBRARY_MAGIC, sizeof(LIBRARY_MAGIC) - 1U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    cursor += sizeof(LIBRARY_MAGIC) - 1U;
    status = archive_read_number(&cursor, end, UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS, &count);
    if (status != UMI_STATUS_OK || count == 0U) return UMI_STATUS_PARSE_ERROR;
    status = archive_read_number(&cursor, end, UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY - 1U, &active_length);
    if (status != UMI_STATUS_OK || active_length == 0U ||
        (size_t)(end - cursor) <= active_length || cursor[active_length] != '\n')
        return UMI_STATUS_PARSE_ERROR;
    *candidate = *host;
    memset(candidate->layouts, 0, sizeof(candidate->layouts));
    candidate->layout_count = 0U;
    memset(candidate->active_layout_id, 0, sizeof(candidate->active_layout_id));
    memcpy(candidate->active_layout_id, cursor, active_length);
    cursor += active_length + 1U;
    for (index = 0U; index < host->layout_count; ++index)
        if (host->layouts[index].revision > maximum_revision) maximum_revision = host->layouts[index].revision;
    text = malloc(UMI_UI_LAYOUT_ENCODED_CAPACITY);
    record = calloc(1U, sizeof(*record));
    snapshot = malloc(sizeof(*snapshot));
    if (text == NULL || record == NULL || snapshot == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY; goto done;
    }
    for (index = 0U; index < count; ++index) {
        size_t bytes;
        status = archive_read_number(&cursor, end, UMI_UI_LAYOUT_ENCODED_CAPACITY - 1U, &bytes);
        if (status != UMI_STATUS_OK || bytes == 0U ||
            (size_t)(end - cursor) <= bytes || cursor[bytes] != '\n') {
            status = UMI_STATUS_PARSE_ERROR; goto done;
        }
        memcpy(text, cursor, bytes);
        text[bytes] = '\0';
        cursor += bytes + 1U;
        if (!umi_document_utf8_validate((const unsigned char *)text, bytes, NULL)) {
            status = UMI_STATUS_PARSE_ERROR; goto done;
        }
        memset(record, 0, sizeof(*record));
        status = umi_ui_layout_persistence_decode(text, record);
        if (status == UMI_STATUS_OK)
            status = umi_ui_checkpoint_internal_validate_record(scope, host, record);
        if (status == UMI_STATUS_OK) status = archive_check_previous(candidate, &record->layout);
        if (status != UMI_STATUS_OK) goto done;
        if (index == 0U) saved_at_ns = record->saved_at_ns;
        else if (saved_at_ns != record->saved_at_ns) { status = UMI_STATUS_PARSE_ERROR; goto done; }
        if (record->layout.revision > maximum_revision) maximum_revision = record->layout.revision;
        if (strcmp(record->layout.layout_id, candidate->active_layout_id) == 0) ++active_matches;
        candidate->layouts[candidate->layout_count++] = record->layout;
    }
    if (cursor != end || active_matches != 1U) { status = UMI_STATUS_PARSE_ERROR; goto done; }
    if (maximum_revision == UINT64_MAX || host->revision == UINT64_MAX) {
        status = UMI_STATUS_CAPACITY_EXCEEDED; goto done;
    }
    for (index = 0U; index < candidate->layout_count; ++index)
        candidate->layouts[index].revision = maximum_revision + 1U;
    candidate->revision = host->revision + 1U;
    /* Each inactive layout must be independently restorable, including group
     * membership limits. Only the selected active projection is published. */
    for (index = 0U; index < candidate->layout_count; ++index) {
        status = archive_project_contexts(host, candidate, &candidate->layouts[index]);
        if (status != UMI_STATUS_OK) goto done;
    }
    status = archive_project_contexts(host, candidate,
        umi_ui_workspace_customisation_active_const(candidate));
    if (status == UMI_STATUS_OK)
        status = umi_ui_workspace_library_snapshot(candidate, &policy, snapshot);
    if (status == UMI_STATUS_OK) {
        memcpy(report->checkpoint.layout_id, candidate->active_layout_id, sizeof(report->checkpoint.layout_id));
        report->checkpoint.saved_at_ns = saved_at_ns;
        report->layout_count = count;
    }
done:
    free(snapshot); free(record); free(text);
    return status;
}

/* Integrity-check and validate before exposing payloads or rotating backups. */
static UmiStatus library_read_validated(const UmiWorkbenchLayoutChunkStore *store,
                                        const char *aggregate,
                                        const UmiUiWorkspaceCheckpointScope *scope,
                                        const UmiUiWorkspaceCustomisation *host,
                                        UmiUiWorkspaceCustomisation *candidate,
                                        char **out_archive,
                                        UmiUiWorkspaceLibraryCheckpointReport *report)
{
    UmiWorkbenchLayoutDataChunkManifest manifest;
    UmiStatus status = umi_workbench_layout_chunk_store_load(store, aggregate, out_archive, &manifest);
    /* NOT_FOUND from the chunk store can name an absent payload chunk, not
     * just an absent archive. Preserve that distinction for the UI's empty
     * store state by checking manifest evidence before returning NOT_FOUND. */
    if (status == UMI_STATUS_NOT_FOUND) {
        UmiUiWorkspaceCheckpointReport evidence = {0};
        const UmiStatus manifest_status = umi_ui_checkpoint_internal_read_revision(
            store->server, aggregate, &evidence);
        status = manifest_status == UMI_STATUS_OK ? UMI_STATUS_PARSE_ERROR : manifest_status;
    }
    /* Chunk payloads have explicit lengths. A forged embedded NUL must not
     * turn an otherwise valid prefix into an accepted, truncated archive. */
    if (status == UMI_STATUS_OK &&
        (manifest.byte_count >= UMI_UI_WORKSPACE_LIBRARY_ARCHIVE_CAPACITY ||
         memchr(*out_archive, '\0', manifest.byte_count) != NULL))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) {
        status = archive_decode(scope, host, *out_archive, candidate, report);
        /* Existing bytes with an unknown tool/context are invalid in this
         * host, never evidence that the user's saved library is absent. */
        if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_INVALID_STATE;
    }
    if (status != UMI_STATUS_OK) { free(*out_archive); *out_archive = NULL; }
    return status;
}

/* Classify resource/backend failures which must not be disguised as recovery. */
static bool infrastructure_failure(UmiStatus status)
{
    return status == UMI_STATUS_OUT_OF_MEMORY || status == UMI_STATUS_IO_ERROR ||
        status == UMI_STATUS_BUSY;
}

/* Save and rotate a complete committed archive in one owned transaction. */
UmiStatus umi_ui_workspace_library_checkpoint_save(
    UmiDataServer *server, const UmiUiWorkspaceCheckpointScope *scope,
    const UmiUiWorkspaceCustomisation *model, uint64_t saved_at_ns,
    uint64_t expected_storage_revision,
    UmiUiWorkspaceLibraryCheckpointReport *out_report)
{
    UmiUiWorkspaceLibraryCheckpointReport report, previous_report;
    UmiUiWorkspaceCustomisation *candidate = NULL;
    UmiWorkbenchLayoutChunkStore store;
    UmiWorkbenchLayoutDataChunkManifest manifest;
    char primary[UMI_UI_CHECKPOINT_AGGREGATE_CAPACITY], backup[UMI_UI_CHECKPOINT_AGGREGATE_CAPACITY];
    char *archive = NULL, *previous = NULL;
    UmiStatus status, primary_status;
    if (storage_overlaps(model, sizeof(*model), out_report, sizeof(*out_report)))
        return UMI_STATUS_INVALID_ARGUMENT;
    library_report_init(server, &report);
    status = umi_ui_checkpoint_internal_library_keys(scope, primary, backup);
    if (status == UMI_STATUS_OK) status = library_validate_host(model, scope);
    if (status == UMI_STATUS_OK && server == NULL) status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK && umi_data_server_in_transaction(server)) status = UMI_STATUS_BUSY;
    if (status != UMI_STATUS_OK) goto done;
    candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    status = archive_encode(scope, model, saved_at_ns, &archive);
    /* Validate the exact framed output, including group capacity, before I/O. */
    if (status == UMI_STATUS_OK) status = archive_decode(scope, model, archive, candidate, &report);
    if (status == UMI_STATUS_OK)
        status = umi_workbench_layout_chunk_store_init(&store, server,
            UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
            UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK);
    if (status == UMI_STATUS_OK) status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) goto done;
    primary_status = umi_ui_checkpoint_internal_read_revision(server, primary, &report.checkpoint);
    report.checkpoint.primary_status = primary_status;
    if (primary_status != UMI_STATUS_OK && primary_status != UMI_STATUS_NOT_FOUND) status = primary_status;
    else if (report.checkpoint.storage_revision != expected_storage_revision) status = UMI_STATUS_INVALID_STATE;
    else if (report.checkpoint.storage_revision == UINT64_MAX) status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK && primary_status == UMI_STATUS_OK) {
        previous_report = report;
        report.checkpoint.primary_status = library_read_validated(&store, primary, scope,
            model, candidate, &previous, &previous_report);
        if (report.checkpoint.primary_status == UMI_STATUS_OK)
            status = umi_workbench_layout_chunk_store_save(&store, backup, previous,
                report.checkpoint.storage_revision,
                previous_report.checkpoint.saved_at_ns / UINT64_C(1000000), NULL);
        else if (infrastructure_failure(report.checkpoint.primary_status))
            status = report.checkpoint.primary_status;
    } else if (status == UMI_STATUS_OK) {
        UmiStatus backup_status = umi_workbench_layout_chunk_store_verify(&store, backup, &manifest);
        if (backup_status == UMI_STATUS_NOT_FOUND)
            status = umi_workbench_layout_chunk_store_save(&store, backup, archive, 1U,
                saved_at_ns / UINT64_C(1000000), NULL);
        else if (infrastructure_failure(backup_status)) status = backup_status;
    }
    if (status == UMI_STATUS_OK)
        status = umi_workbench_layout_chunk_store_save(&store, primary, archive,
            report.checkpoint.storage_revision + 1U, saved_at_ns / UINT64_C(1000000), NULL);
    status = umi_ui_checkpoint_internal_finish_transaction(server, status);
    if (status == UMI_STATUS_OK) {
        ++report.checkpoint.storage_revision;
        report.checkpoint.storage_revision_known = true;
    }
done:
    free(previous); free(archive); free(candidate);
    if (out_report != NULL) *out_report = report;
    return status;
}

/* Publish a candidate only after complete validation and read transaction
 * completion; the host decides whether its native presentation accepts it. */
UmiStatus umi_ui_workspace_library_checkpoint_load_candidate(
    UmiDataServer *server, const UmiUiWorkspaceCheckpointScope *scope,
    const UmiUiWorkspaceCustomisation *validation_model,
    UmiUiWorkspaceCustomisation *out_candidate,
    UmiUiWorkspaceLibraryCheckpointReport *out_report)
{
    UmiUiWorkspaceLibraryCheckpointReport report;
    UmiUiWorkspaceCustomisation *candidate = NULL;
    UmiWorkbenchLayoutChunkStore store;
    char primary[UMI_UI_CHECKPOINT_AGGREGATE_CAPACITY], backup[UMI_UI_CHECKPOINT_AGGREGATE_CAPACITY];
    char *archive = NULL;
    UmiStatus status;
    if (storage_overlaps(validation_model, sizeof(*validation_model), out_candidate, sizeof(*out_candidate)) ||
        storage_overlaps(validation_model, sizeof(*validation_model), out_report, sizeof(*out_report)) ||
        storage_overlaps(out_candidate, sizeof(*out_candidate), out_report, sizeof(*out_report)))
        return UMI_STATUS_INVALID_ARGUMENT;
    library_report_init(server, &report);
    status = umi_ui_checkpoint_internal_library_keys(scope, primary, backup);
    if (status == UMI_STATUS_OK) status = library_validate_host(validation_model, scope);
    if (status == UMI_STATUS_OK && (server == NULL || out_candidate == NULL)) status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK && umi_data_server_in_transaction(server)) status = UMI_STATUS_BUSY;
    if (status != UMI_STATUS_OK) goto done;
    candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    status = umi_workbench_layout_chunk_store_init(&store, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK);
    if (status == UMI_STATUS_OK) status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) goto done;
    (void)umi_ui_checkpoint_internal_read_revision(server, primary, &report.checkpoint);
    report.checkpoint.primary_status = library_read_validated(&store, primary, scope,
        validation_model, candidate, &archive, &report);
    status = report.checkpoint.primary_status;
    if (status != UMI_STATUS_OK && !infrastructure_failure(status)) {
        UmiStatus backup_status = library_read_validated(&store, backup, scope,
            validation_model, candidate, &archive, &report);
        if (backup_status == UMI_STATUS_OK) {
            status = UMI_STATUS_OK;
            report.checkpoint.recovered_last_good = true;
        } else if (report.checkpoint.primary_status == UMI_STATUS_NOT_FOUND) status = backup_status;
    }
    status = umi_ui_checkpoint_internal_finish_transaction(server, status);
    if (status == UMI_STATUS_OK) *out_candidate = *candidate;
done:
    free(archive); free(candidate);
    if (out_report != NULL) *out_report = report;
    return status;
}
