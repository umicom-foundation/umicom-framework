/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workspace_checkpoint.c
 *
 * PURPOSE:
 *   Persist the existing native workspace codec through Data Server chunks,
 *   with explicit save revisions, validated recovery and a last-good copy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/workspace_checkpoint.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/geometry.h"
#include "umicom/application/suite_layout/render_plan.h"
#include "umicom/document/text_encoding.h"
#include "umicom/workbench_layout_data/chunk_store.h"
#include "umicom/workbench_layout_data/key_codec.h"

#define CHECKPOINT_SCOPE_CAPACITY 64U
#define CHECKPOINT_AGGREGATE_CAPACITY 152U
#define CHECKPOINT_REVISION_RESERVE (4U * UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS + 8U)

/* Validate finite C-array text before string comparisons or codec operations. */
static bool checked_text(const char *text, size_t capacity, bool allow_empty)
{
    const char *end = text != NULL ? memchr(text, '\0', capacity) : NULL;
    return end != NULL && (allow_empty || end != text) &&
        umi_document_utf8_validate((const unsigned char *)text, (size_t)(end - text), NULL) != 0;
}

/* The @ separator is reserved by this service, so product/workspace pairs
 * cannot collide by placing a delimiter inside an individual identifier. */
static bool scope_id_valid(const char *text)
{
    size_t index;
    if (text == NULL) return false;
    for (index = 0U; index < CHECKPOINT_SCOPE_CAPACITY; ++index) {
        const unsigned char c = (unsigned char)text[index];
        if (c == 0U) return index != 0U;
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '.' || c == '_' || c == '-')) return false;
    }
    return false;
}

/* Construct two separate records inside the established key codec. This is
 * key scoping only: the layout payload remains the existing UI codec. */
static UmiStatus checkpoint_keys(const UmiUiWorkspaceCheckpointScope *scope,
                                char *primary, char *backup)
{
    size_t prefix_length;
    int written;
    if (scope == NULL || !scope_id_valid(scope->application_id) ||
        !scope_id_valid(scope->workspace_id) || scope->layout_prefix == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    prefix_length = 0U;
    while (prefix_length < UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY &&
           scope->layout_prefix[prefix_length] != '\0') ++prefix_length;
    if (prefix_length == 0U || prefix_length >= UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY ||
        scope->layout_prefix[prefix_length - 1U] != '.') return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(primary, CHECKPOINT_AGGREGATE_CAPACITY, "%s@%s@primary",
        scope->application_id, scope->workspace_id);
    if (written < 0 || (size_t)written >= CHECKPOINT_AGGREGATE_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    written = snprintf(backup, CHECKPOINT_AGGREGATE_CAPACITY, "%s@%s@last-good",
        scope->application_id, scope->workspace_id);
    return written < 0 || (size_t)written >= CHECKPOINT_AGGREGATE_CAPACITY
        ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

/* Report storage capability even when a database does not yet contain a save. */
static void checkpoint_report_init(UmiDataServer *server, UmiUiWorkspaceCheckpointReport *report)
{
    const char *path = server != NULL ? umi_data_server_path(server) : NULL;
    memset(report, 0, sizeof(*report));
    report->primary_status = UMI_STATUS_NOT_FOUND;
    report->durable = server != NULL && umi_data_server_backend(server) == UMI_DATA_BACKEND_SQLITE &&
        path != NULL && path[0] != '\0' && strcmp(path, ":memory:") != 0;
}

/* Every borrowed catalogue must be safe to search before the existing importer
 * is called. Reserve enough revisions for context reconciliation and selection. */
static UmiStatus validate_host(const UmiUiWorkspaceCustomisation *model)
{
    size_t index, other;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (model->edit_active) return UMI_STATUS_BUSY;
    if (model->layout_count > UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS ||
        model->windows.count > UMI_UI_WINDOW_CATALOGUE_MAX || model->groups.count > UMI_UI_WINDOW_GROUP_MAX)
        return UMI_STATUS_INVALID_STATE;
    if (!checked_text(model->active_layout_id, sizeof(model->active_layout_id), true))
        return UMI_STATUS_INVALID_STATE;
    if (model->revision > UINT64_MAX - CHECKPOINT_REVISION_RESERVE ||
        model->groups.revision > UINT64_MAX - CHECKPOINT_REVISION_RESERVE)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (index = 0U; index < model->layout_count; ++index) {
        const UmiUiWorkspaceLayout *layout = &model->layouts[index];
        if (!checked_text(layout->layout_id, sizeof(layout->layout_id), false) ||
            layout->window_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS) return UMI_STATUS_INVALID_STATE;
        for (other = 0U; other < layout->window_count; ++other) {
            const UmiUiWorkspaceWindow *window = &layout->windows[other];
            if (!checked_text(window->window_id, sizeof(window->window_id), false) ||
                !checked_text(window->tool_id, sizeof(window->tool_id), false)) return UMI_STATUS_INVALID_STATE;
        }
    }
    for (index = 0U; index < model->windows.count; ++index)
        if (!checked_text(model->windows.items[index].tool_id,
            sizeof(model->windows.items[index].tool_id), false)) return UMI_STATUS_INVALID_STATE;
    for (index = 0U; index < model->groups.count; ++index) {
        const UmiUiWindowGroup *group = &model->groups.items[index];
        if (!checked_text(group->group_id, sizeof(group->group_id), false) ||
            group->member_count > UMI_UI_WINDOW_GROUP_MAX_MEMBERS) return UMI_STATUS_INVALID_STATE;
        if (group->revision > UINT64_MAX - CHECKPOINT_REVISION_RESERVE) return UMI_STATUS_CAPACITY_EXCEEDED;
        for (other = 0U; other < group->member_count; ++other)
            if (!checked_text(group->members[other].window_id,
                sizeof(group->members[other].window_id), false)) return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* Product flags are Framework metadata. The native Studio explorer retains
 * its established historical alias; other tools use exact canonical IDs. */
static UmiStatus validate_product_panel(const UmiUiWorkspaceCheckpointScope *scope,
                                       const UmiUiWorkspaceWindow *window)
{
    const UmiApplicationExperienceDefinition *experience =
        umi_application_experience_catalogue_find(scope->application_id);
    const char *panel_id = strcmp(scope->application_id, "org.umicom.studio") == 0 &&
        strcmp(window->tool_id, "explorer") == 0 ? "resource-explorer" : window->tool_id;
    const UmiExperiencePanelDefinition *panel = experience != NULL
        ? umi_application_experience_panel_find(experience, panel_id) : NULL;
    const uint32_t required = window->floating ? UMI_EXPERIENCE_PANEL_FLOATABLE : UMI_EXPERIENCE_PANEL_DOCKABLE;
    /* This record is not active in the validation host yet, so inspect the
     * same persisted prefix used by the existing customisation query. */
    const bool auto_hidden = strncmp(window->placement_id, "auto-hide:", sizeof("auto-hide:") - 1U) == 0;
    /* The existing placement prefix is a docked-only presentation state;
     * importing text must not bypass the product's Auto Hide capability. */
    if (auto_hidden && window->floating) return UMI_STATUS_PERMISSION_DENIED;
    if (panel == NULL) return UMI_STATUS_OK;
    if ((panel->flags & required) == 0U || (window->context_group_id[0] != '\0' &&
        (panel->flags & UMI_EXPERIENCE_PANEL_CONTEXT_LINKED) == 0U) ||
        (auto_hidden && (panel->flags & UMI_EXPERIENCE_PANEL_AUTO_HIDE) == 0U)) return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_OK;
}

/* Validate every record, including hidden cards that could later be reopened.
 * Singleton IDs cannot be forged to mount the same native editor body twice. */
static UmiStatus validate_record(const UmiUiWorkspaceCheckpointScope *scope,
                                const UmiUiWorkspaceCustomisation *model,
                                const UmiUiLayoutPersistenceRecord *record)
{
    const UmiUiWorkspaceLayout *layout = &record->layout;
    const size_t prefix_length = strlen(scope->layout_prefix);
    size_t index, other, saved;
    if (record->schema_version != UMI_UI_LAYOUT_PERSISTENCE_SCHEMA_VERSION ||
        !checked_text(layout->layout_id, sizeof(layout->layout_id), false) ||
        !checked_text(layout->name, sizeof(layout->name), false) ||
        strncmp(layout->layout_id, scope->layout_prefix, prefix_length) != 0 ||
        layout->layout_id[prefix_length] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (!layout->locked || layout->window_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS)
        return UMI_STATUS_INVALID_STATE;
    if (layout->revision > UINT64_MAX - CHECKPOINT_REVISION_RESERVE)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (index = 0U; index < layout->window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &layout->windows[index];
        const UmiUiWindowDescriptor *descriptor;
        UmiApplicationSuiteLayoutRect rect = {window->x, window->y, window->width, window->height};
        UmiStatus status;
        if (!checked_text(window->window_id, sizeof(window->window_id), false) ||
            !checked_text(window->tool_id, sizeof(window->tool_id), false) ||
            !checked_text(window->title, sizeof(window->title), false) ||
            !checked_text(window->placement_id, sizeof(window->placement_id), false) ||
            !checked_text(window->group_id, sizeof(window->group_id), true) ||
            !checked_text(window->stack_id, sizeof(window->stack_id), true) ||
            !checked_text(window->context_group_id, sizeof(window->context_group_id), true) ||
            !umi_application_suite_layout_canvas_rect_valid(&rect)) return UMI_STATUS_INVALID_STATE;
        descriptor = umi_ui_window_catalogue_find(&model->windows, window->tool_id);
        if (descriptor == NULL) return UMI_STATUS_NOT_FOUND;
        if (!descriptor->supports_multiple && strcmp(window->window_id, window->tool_id) != 0)
            return UMI_STATUS_INVALID_STATE;
        for (other = 0U; other < index; ++other)
            if (strcmp(layout->windows[other].window_id, window->window_id) == 0 ||
                (!descriptor->supports_multiple && strcmp(layout->windows[other].tool_id, window->tool_id) == 0))
                return UMI_STATUS_INVALID_STATE;
        for (saved = 0U; saved < model->layout_count; ++saved) {
            const UmiUiWorkspaceWindow *existing =
                umi_ui_workspace_layout_find_window(&model->layouts[saved], window->window_id);
            if (existing != NULL && strcmp(existing->tool_id, window->tool_id) != 0)
                return UMI_STATUS_INVALID_STATE;
        }
        status = validate_product_panel(scope, window);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/* Reuse decode, render projection and atomic import validation. Re-encoding
 * rejects trailing numeric junk or noncanonical booleans accepted by older
 * permissive readers, without defining another workspace file grammar. */
static UmiStatus validate_text(const UmiUiWorkspaceCheckpointScope *scope,
                              const UmiUiWorkspaceCustomisation *model,
                              const char *text, UmiUiWorkspaceCheckpointReport *report)
{
    UmiUiLayoutPersistenceRecord *record;
    UmiApplicationSuiteLayoutRenderPlan *plan;
    UmiUiWorkspaceCustomisation *candidate;
    UmiUiWorkspaceImportOptions options = umi_ui_workspace_import_options_default();
    UmiStatus status;
    /* ChunkStore owns a terminated allocation sized to this text; do not scan
     * it as though it were the larger fixed-capacity encoder output buffer. */
    const size_t text_length = strlen(text);
    if (text_length == 0U || text_length >= UMI_UI_LAYOUT_ENCODED_CAPACITY ||
        !umi_document_utf8_validate((const unsigned char *)text, text_length, NULL)) return UMI_STATUS_PARSE_ERROR;
    record = calloc(1U, sizeof(*record));
    plan = malloc(sizeof(*plan));
    candidate = malloc(sizeof(*candidate));
    if (record == NULL || plan == NULL || candidate == NULL) {
        free(record); free(plan); free(candidate);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    status = umi_ui_layout_persistence_decode(text, record);
    if (status == UMI_STATUS_OK) status = validate_record(scope, model, record);
    if (status == UMI_STATUS_OK)
        status = umi_application_suite_layout_render_plan_build(&record->layout, plan);
    if (status == UMI_STATUS_OK) {
        *candidate = *model;
        options.allow_new_layout = true;
        options.conflict_policy = UMI_UI_WORKSPACE_IMPORT_REPLACE_CONFLICT;
        status = umi_ui_workspace_customisation_import(candidate, text, &options, NULL);
    }
    if (status == UMI_STATUS_OK && report != NULL) {
        memcpy(report->layout_id, record->layout.layout_id, sizeof(report->layout_id));
        report->saved_at_ns = record->saved_at_ns;
    }
    free(candidate); free(plan); free(record);
    return status;
}

/* Check the raw active record before the encoder searches its fixed strings.
 * Live model memory is a public C contract and may be malformed independently
 * of any persisted text, so encode-then-validate alone is not sufficient. */
static UmiStatus validate_active(const UmiUiWorkspaceCheckpointScope *scope,
                                 const UmiUiWorkspaceCustomisation *model)
{
    const UmiUiWorkspaceLayout *active = umi_ui_workspace_customisation_active_const(model);
    UmiUiLayoutPersistenceRecord *record;
    UmiStatus status;
    if (active == NULL) return UMI_STATUS_NOT_FOUND;
    record = calloc(1U, sizeof(*record));
    if (record == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    record->schema_version = UMI_UI_LAYOUT_PERSISTENCE_SCHEMA_VERSION;
    record->layout = *active;
    status = validate_record(scope, model, record);
    free(record);
    return status;
}

/* Decode only the bounded manifest to establish a trustworthy CAS revision.
 * A damaged payload can recover from backup; unknown manifest identity or
 * revision cannot authorise overwriting the primary during a later Save. */
static UmiStatus read_primary_revision(UmiDataServer *server, const char *primary,
                                       UmiUiWorkspaceCheckpointReport *report)
{
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    UmiWorkbenchLayoutDataChunkManifest manifest;
    UmiStatus status = umi_workbench_layout_data_key_build(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST, primary, NULL, 0U, 0U, key, sizeof(key));
    if (status == UMI_STATUS_OK) status = umi_data_server_get(server, key, value, sizeof(value));
    if (status == UMI_STATUS_NOT_FOUND) {
        report->storage_revision_known = true;
        report->storage_revision = 0U;
        return status;
    }
    if (status == UMI_STATUS_OK) status = umi_workbench_layout_chunk_manifest_decode(value, &manifest);
    if (status == UMI_STATUS_OK && (strcmp(manifest.aggregate_id, primary) != 0 ||
        manifest.manifest_kind != UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST ||
        manifest.chunk_kind != UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK)) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) {
        report->storage_revision_known = true;
        report->storage_revision = manifest.revision;
    }
    return status;
}

/* Complete only this service's transaction; never commit a caller's work. */
static UmiStatus finish_transaction(UmiDataServer *server, UmiStatus status)
{
    if (status == UMI_STATUS_OK) status = umi_data_server_commit(server);
    if (status != UMI_STATUS_OK && umi_data_server_in_transaction(server)) {
        UmiStatus rollback_status = umi_data_server_rollback(server);
        if (rollback_status != UMI_STATUS_OK) return rollback_status;
    }
    return status;
}

/* Decode and validate a chunk payload before returning it to either save
 * rotation or the presentation host. The chunk store checks its byte hash. */
static UmiStatus read_validated(const UmiWorkbenchLayoutChunkStore *store,
                                const char *aggregate, const UmiUiWorkspaceCheckpointScope *scope,
                                const UmiUiWorkspaceCustomisation *model, char **out_text,
                                UmiUiWorkspaceCheckpointReport *report)
{
    UmiStatus status = umi_workbench_layout_chunk_store_load(store, aggregate, out_text, NULL);
    if (status == UMI_STATUS_OK) status = validate_text(scope, model, *out_text, report);
    if (status != UMI_STATUS_OK) { free(*out_text); *out_text = NULL; }
    return status;
}

/* Save a validated committed snapshot and rotate its predecessor atomically. */
UmiStatus umi_ui_workspace_checkpoint_save(
    UmiDataServer *server, const UmiUiWorkspaceCheckpointScope *scope,
    const UmiUiWorkspaceCustomisation *model, uint64_t saved_at_ns,
    uint64_t expected_storage_revision, UmiUiWorkspaceCheckpointReport *out_report)
{
    UmiUiWorkspaceCheckpointReport report, previous_report;
    UmiWorkbenchLayoutChunkStore store;
    UmiWorkbenchLayoutDataChunkManifest manifest;
    char primary[CHECKPOINT_AGGREGATE_CAPACITY], backup[CHECKPOINT_AGGREGATE_CAPACITY];
    char *text = NULL, *previous = NULL;
    UmiStatus status, primary_status;
    checkpoint_report_init(server, &report);
    status = checkpoint_keys(scope, primary, backup);
    if (status == UMI_STATUS_OK) status = validate_host(model);
    if (status == UMI_STATUS_OK && server == NULL) status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK && umi_data_server_in_transaction(server)) status = UMI_STATUS_BUSY;
    if (status == UMI_STATUS_OK) status = validate_active(scope, model);
    if (status != UMI_STATUS_OK) goto done;
    text = malloc(UMI_UI_LAYOUT_ENCODED_CAPACITY);
    if (text == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    status = umi_ui_workspace_customisation_export_active(model, saved_at_ns, text, UMI_UI_LAYOUT_ENCODED_CAPACITY);
    if (status == UMI_STATUS_OK) status = validate_text(scope, model, text, &report);
    if (status == UMI_STATUS_OK) status = umi_workbench_layout_chunk_store_init(&store, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST, UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK);
    if (status != UMI_STATUS_OK) goto done;
    status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) goto done;
    primary_status = read_primary_revision(server, primary, &report);
    report.primary_status = primary_status;
    if (primary_status != UMI_STATUS_OK && primary_status != UMI_STATUS_NOT_FOUND) status = primary_status;
    else if (report.storage_revision != expected_storage_revision) status = UMI_STATUS_INVALID_STATE;
    else if (report.storage_revision == UINT64_MAX) status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK && primary_status == UMI_STATUS_OK) {
        previous_report = report;
        report.primary_status = read_validated(&store, primary, scope, model, &previous, &previous_report);
        /* A corrupt primary is never promoted over the good backup. A valid
         * manifest revision still allows an explicit, correctly based Save. */
        if (report.primary_status == UMI_STATUS_OK)
            status = umi_workbench_layout_chunk_store_save(&store, backup, previous,
                report.storage_revision, previous_report.saved_at_ns / UINT64_C(1000000), NULL);
        else if (report.primary_status == UMI_STATUS_OUT_OF_MEMORY || report.primary_status == UMI_STATUS_IO_ERROR)
            status = report.primary_status;
    } else if (status == UMI_STATUS_OK) {
        UmiStatus backup_status = umi_workbench_layout_chunk_store_verify(&store, backup, &manifest);
        /* The first save also establishes a recovery copy. An existing backup
         * is retained when a primary was removed rather than silently reset. */
        if (backup_status == UMI_STATUS_NOT_FOUND)
            status = umi_workbench_layout_chunk_store_save(&store, backup, text, 1U,
                saved_at_ns / UINT64_C(1000000), NULL);
        else if (backup_status == UMI_STATUS_OUT_OF_MEMORY || backup_status == UMI_STATUS_IO_ERROR) status = backup_status;
    }
    if (status == UMI_STATUS_OK)
        status = umi_workbench_layout_chunk_store_save(&store, primary, text,
            report.storage_revision + 1U, saved_at_ns / UINT64_C(1000000), NULL);
    status = finish_transaction(server, status);
    if (status == UMI_STATUS_OK) {
        report.storage_revision += 1U;
        report.storage_revision_known = true;
    }
done:
    free(previous); free(text);
    if (out_report != NULL) *out_report = report;
    return status;
}

/* Read consistent primary/backup records, publishing text only after full
 * validation and completion of this service's read-only transaction. */
UmiStatus umi_ui_workspace_checkpoint_load_validated(
    UmiDataServer *server, const UmiUiWorkspaceCheckpointScope *scope,
    const UmiUiWorkspaceCustomisation *validation_model,
    char *out_text, size_t capacity, UmiUiWorkspaceCheckpointReport *out_report)
{
    UmiUiWorkspaceCheckpointReport report;
    UmiWorkbenchLayoutChunkStore store;
    char primary[CHECKPOINT_AGGREGATE_CAPACITY], backup[CHECKPOINT_AGGREGATE_CAPACITY];
    char *text = NULL;
    UmiStatus status;
    checkpoint_report_init(server, &report);
    status = checkpoint_keys(scope, primary, backup);
    if (status == UMI_STATUS_OK) status = validate_host(validation_model);
    if (status == UMI_STATUS_OK && (server == NULL || out_text == NULL || capacity == 0U))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK && umi_data_server_in_transaction(server)) status = UMI_STATUS_BUSY;
    if (status != UMI_STATUS_OK) goto done;
    status = umi_workbench_layout_chunk_store_init(&store, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST, UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK);
    if (status == UMI_STATUS_OK) status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) goto done;
    (void)read_primary_revision(server, primary, &report);
    report.primary_status = read_validated(&store, primary, scope, validation_model, &text, &report);
    status = report.primary_status;
    if (status != UMI_STATUS_OK && status != UMI_STATUS_OUT_OF_MEMORY && status != UMI_STATUS_IO_ERROR) {
        UmiStatus backup_status = read_validated(&store, backup, scope, validation_model, &text, &report);
        if (backup_status == UMI_STATUS_OK) { status = UMI_STATUS_OK; report.recovered_last_good = true; }
        else if (report.primary_status == UMI_STATUS_NOT_FOUND) status = backup_status;
    }
    if (status == UMI_STATUS_OK && strlen(text) >= capacity) status = UMI_STATUS_CAPACITY_EXCEEDED;
    status = finish_transaction(server, status);
    if (status == UMI_STATUS_OK) memcpy(out_text, text, strlen(text) + 1U);
done:
    free(text);
    if (out_report != NULL) *out_report = report;
    return status;
}

/* Restore into a heap candidate so failed storage, parsing or context repair
 * cannot invalidate a live layout or the caller's current selection. */
UmiStatus umi_ui_workspace_checkpoint_restore(
    UmiDataServer *server, const UmiUiWorkspaceCheckpointScope *scope,
    UmiUiWorkspaceCustomisation *model, UmiUiWorkspaceCheckpointReport *out_report)
{
    UmiUiWorkspaceCheckpointReport report;
    UmiUiWorkspaceCustomisation *candidate = NULL;
    UmiUiWorkspaceImportOptions options = umi_ui_workspace_import_options_default();
    char *text = NULL;
    UmiStatus status;
    checkpoint_report_init(server, &report);
    status = validate_host(model);
    if (status != UMI_STATUS_OK) goto done;
    candidate = malloc(sizeof(*candidate));
    text = malloc(UMI_UI_LAYOUT_ENCODED_CAPACITY);
    if (candidate == NULL || text == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    *candidate = *model;
    status = umi_ui_workspace_checkpoint_load_validated(server, scope, candidate,
        text, UMI_UI_LAYOUT_ENCODED_CAPACITY, &report);
    options.allow_new_layout = true;
    options.conflict_policy = UMI_UI_WORKSPACE_IMPORT_REPLACE_CONFLICT;
    if (status == UMI_STATUS_OK)
        status = umi_ui_workspace_customisation_import(candidate, text, &options, NULL);
    if (status == UMI_STATUS_OK) *model = *candidate;
done:
    free(text); free(candidate);
    if (out_report != NULL) *out_report = report;
    return status;
}
