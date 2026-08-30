/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/service.c
 *
 * PURPOSE:
 *   Implement the authoritative layout persistence and synchronisation service with transactional revision, change-feed and outbox evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "internal.h"



struct UmiWorkbenchLayoutDataService {
    UmiDataServer *server;
    UmiWorkbenchLayoutDataServiceConfig config;
    UmiWorkbenchLayoutDataServerStore store;
    UmiWorkbenchLayoutRevisionStoreRepository revisions;
    UmiWorkbenchLayoutGrantStoreRepository grants;
    UmiWorkbenchLayoutLeaseStoreRepository leases;
    UmiWorkbenchLayoutChangeFeedRepository changes;
    UmiWorkbenchLayoutOutboxRepository outbox;
    UmiWorkbenchLayoutConflictStoreRepository conflicts;
    UmiWorkbenchLayoutSyncCursorRepository cursors;
    UmiWorkbenchLayoutPresenceStoreRepository presence;
    UmiWorkbenchLayoutMetricsCollector metrics;
    UmiWorkbenchLayoutSyncEngine sync;
    uint64_t next_sequence;
    uint64_t last_success_at_ms;
    uint64_t last_failure_at_ms;
    uint64_t revision;
    bool started;
    bool accepting_writes;
};

UmiWorkbenchLayoutDataServiceConfig
umi_workbench_layout_data_service_config_default(void)
{
    UmiWorkbenchLayoutDataServiceConfig config;
    (void)memset(&config, 0, sizeof(config));
    config.structure_size = sizeof(config);
    (void)umi_workbench_layout_data_copy_text(
        config.replica_id, sizeof(config.replica_id),
        "local", false);
    (void)umi_workbench_layout_data_copy_text(
        config.actor_id, sizeof(config.actor_id),
        "system", false);
    config.lease_duration_ms = UINT64_C(30000);
    config.presence_ttl_ms = UINT64_C(45000);
    config.outbox_lease_ms = UINT64_C(30000);
    config.offline_claim_ms = UINT64_C(30000);
    config.sync_policy = umi_workbench_layout_sync_policy_default();
    config.health_policy = umi_workbench_layout_health_policy_default();
    return config;
}

static UmiStatus initialise_repositories(
    UmiWorkbenchLayoutDataService *service)
{
    UmiStatus status;
    status = umi_workbench_layout_data_server_store_init(
        &service->store, service->server);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_revision_store_repository_init(
            &service->revisions, service->server);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_grant_store_repository_init(
            &service->grants, service->server);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_lease_store_repository_init(
            &service->leases, service->server);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_change_feed_repository_init(
            &service->changes, service->server);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_outbox_repository_init(
            &service->outbox, service->server);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_conflict_store_repository_init(
            &service->conflicts, service->server);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_sync_cursor_repository_init(
            &service->cursors, service->server);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_presence_store_repository_init(
            &service->presence, service->server);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_sync_engine_init(
            &service->sync,
            &service->conflicts,
            &service->cursors,
            &service->config.sync_policy);
    }
    return status;
}

UmiStatus umi_workbench_layout_data_service_create(
    UmiDataServer *server,
    const UmiWorkbenchLayoutDataServiceConfig *config,
    UmiWorkbenchLayoutDataService **out_service)
{
    UmiWorkbenchLayoutDataService *service;
    UmiWorkbenchLayoutDataServiceConfig effective;
    UmiStatus status;
    if (server == NULL || out_service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_service = NULL;
    effective = config != NULL
        ? *config : umi_workbench_layout_data_service_config_default();
    if (effective.structure_size < sizeof(effective) ||
        effective.replica_id[0] == '\0' ||
        effective.actor_id[0] == '\0' ||
        effective.lease_duration_ms == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    service = (UmiWorkbenchLayoutDataService *)calloc(
        1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->server = server;
    service->config = effective;
    service->next_sequence = 1U;
    service->revision = 1U;
    umi_workbench_layout_metrics_init(&service->metrics);
    status = initialise_repositories(service);
    if (status != UMI_STATUS_OK) {
        free(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_workbench_layout_data_service_destroy(
    UmiWorkbenchLayoutDataService *service)
{
    free(service);
}

UmiStatus umi_workbench_layout_data_service_start(
    UmiWorkbenchLayoutDataService *service,
    uint64_t now_ms)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (service->started) return UMI_STATUS_OK;
    service->started = true;
    service->accepting_writes = true;
    service->last_success_at_ms = now_ms;
    service->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_data_service_quiesce(
    UmiWorkbenchLayoutDataService *service)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!service->started) return UMI_STATUS_INVALID_STATE;
    service->accepting_writes = false;
    service->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_data_service_stop(
    UmiWorkbenchLayoutDataService *service,
    uint64_t now_ms)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    service->accepting_writes = false;
    service->started = false;
    service->last_success_at_ms = now_ms;
    service->revision += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus require_write(
    const UmiWorkbenchLayoutDataService *service)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!service->started || !service->accepting_writes) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

static UmiStatus append_change_and_outbox(
    UmiWorkbenchLayoutDataService *service,
    const char *layout_id,
    UmiWorkbenchLayoutDataChangeKind kind,
    uint64_t revision,
    uint64_t base_revision,
    uint64_t content_hash,
    const char *correlation_id,
    const char *summary,
    uint64_t now_ms)
{
    UmiWorkbenchLayoutChange change;
    UmiWorkbenchLayoutOutboxEntry outbox;
    UmiStatus status;
    const uint64_t sequence = service->next_sequence++;
    (void)memset(&change, 0, sizeof(change));
    change.structure_size = sizeof(change);
    change.kind = kind;
    change.sequence = sequence;
    change.revision = revision;
    change.base_revision = base_revision;
    change.content_hash = content_hash;
    change.occurred_at_ms = now_ms;
    (void)umi_workbench_layout_data_make_record_id(
        "change", layout_id, sequence,
        change.change_id, sizeof(change.change_id));
    (void)umi_workbench_layout_data_copy_text(
        change.layout_id, sizeof(change.layout_id),
        layout_id, false);
    (void)umi_workbench_layout_data_copy_text(
        change.actor_id, sizeof(change.actor_id),
        service->config.actor_id, false);
    (void)umi_workbench_layout_data_copy_text(
        change.correlation_id, sizeof(change.correlation_id),
        correlation_id != NULL ? correlation_id : "", true);
    (void)umi_workbench_layout_data_copy_text(
        change.summary, sizeof(change.summary),
        summary != NULL ? summary : "", true);
    status = umi_workbench_layout_change_feed_save(
        &service->changes, &change);
    if (status != UMI_STATUS_OK) return status;

    (void)memset(&outbox, 0, sizeof(outbox));
    outbox.structure_size = sizeof(outbox);
    outbox.state = UMI_WORKBENCH_LAYOUT_DATA_OUTBOX_PENDING;
    outbox.sequence = sequence;
    outbox.created_at_ms = now_ms;
    outbox.available_at_ms = now_ms;
    outbox.maximum_attempts = 8U;
    (void)umi_workbench_layout_data_make_record_id(
        "outbox", layout_id, sequence,
        outbox.entry_id, sizeof(outbox.entry_id));
    (void)umi_workbench_layout_data_copy_text(
        outbox.topic, sizeof(outbox.topic),
        "umicom.workbench.layout.changed", false);
    (void)umi_workbench_layout_data_copy_text(
        outbox.aggregate_id, sizeof(outbox.aggregate_id),
        layout_id, false);
    (void)umi_workbench_layout_data_copy_text(
        outbox.correlation_id, sizeof(outbox.correlation_id),
        correlation_id != NULL ? correlation_id : "", true);
    (void)umi_workbench_layout_data_copy_text(
        outbox.payload_reference,
        sizeof(outbox.payload_reference),
        change.change_id, false);
    return umi_workbench_layout_outbox_save(
        &service->outbox, &outbox);
}

static UmiStatus append_revision(
    UmiWorkbenchLayoutDataService *service,
    const UmiWorkbenchLayoutDocument *document,
    uint64_t base_revision,
    const char *correlation_id,
    const char *summary,
    uint64_t now_ms,
    bool deletion)
{
    UmiWorkbenchLayoutRevisionRecord record;
    (void)memset(&record, 0, sizeof(record));
    record.structure_size = sizeof(record);
    record.revision = document->version.revision;
    record.base_revision = base_revision;
    record.content_hash = document->content_hash;
    record.created_at_ms = now_ms;
    record.deletion = deletion;
    (void)umi_workbench_layout_data_make_record_id(
        "revision", document->identity.layout_id,
        record.revision,
        record.revision_id, sizeof(record.revision_id));
    (void)umi_workbench_layout_data_copy_text(
        record.layout_id, sizeof(record.layout_id),
        document->identity.layout_id, false);
    (void)umi_workbench_layout_data_copy_text(
        record.actor_id, sizeof(record.actor_id),
        service->config.actor_id, false);
    (void)umi_workbench_layout_data_copy_text(
        record.correlation_id, sizeof(record.correlation_id),
        correlation_id != NULL ? correlation_id : "", true);
    (void)umi_workbench_layout_data_copy_text(
        record.summary, sizeof(record.summary),
        summary != NULL ? summary : "", true);
    return umi_workbench_layout_revision_store_save(
        &service->revisions, &record);
}

UmiStatus umi_workbench_layout_data_service_save_layout(
    UmiWorkbenchLayoutDataService *service,
    UmiWorkbenchLayoutDocument *document,
    uint64_t expected_revision,
    const char *correlation_id,
    uint64_t now_ms,
    uint64_t *out_revision)
{
    UmiWorkbenchLayoutStoreAdapter adapter;
    uint64_t resulting_revision = 0U;
    bool transaction_started = false;
    UmiStatus status = require_write(service);
    if (status != UMI_STATUS_OK || document == NULL) return status;
    adapter = umi_workbench_layout_data_server_store_adapter(
        &service->store);
    status = umi_workbench_layout_data_transaction_begin(
        service->server, &transaction_started);
    if (status == UMI_STATUS_OK) {
        document->audit.modified_at_ms = now_ms;
        status = umi_workbench_layout_store_save(
            &adapter, document, expected_revision,
            &resulting_revision);
    }
    if (status == UMI_STATUS_OK) {
        document->version.base_revision = expected_revision;
        document->version.revision = resulting_revision;
        status = append_revision(
            service, document, expected_revision,
            correlation_id, "Layout saved.", now_ms, false);
    }
    if (status == UMI_STATUS_OK) {
        status = append_change_and_outbox(
            service, document->identity.layout_id,
            expected_revision == 0U
                ? UMI_WORKBENCH_LAYOUT_DATA_CHANGE_CREATED
                : UMI_WORKBENCH_LAYOUT_DATA_CHANGE_UPDATED,
            resulting_revision, expected_revision,
            document->content_hash, correlation_id,
            "Layout state changed.", now_ms);
    }
    status = umi_workbench_layout_data_transaction_finish(
        service->server, transaction_started, status);
    if (status == UMI_STATUS_OK) {
        service->last_success_at_ms = now_ms;
        service->revision += 1U;
        umi_workbench_layout_metrics_record_save(
            &service->metrics, sizeof(*document), 0U, status);
        if (out_revision != NULL) *out_revision = resulting_revision;
    } else {
        service->last_failure_at_ms = now_ms;
        umi_workbench_layout_metrics_record_save(
            &service->metrics, 0U, 0U, status);
    }
    return status;
}

UmiStatus umi_workbench_layout_data_service_load_layout(
    UmiWorkbenchLayoutDataService *service,
    const char *layout_id,
    UmiWorkbenchLayoutDocument *out_document)
{
    UmiWorkbenchLayoutStoreAdapter adapter;
    UmiStatus status;
    if (service == NULL || layout_id == NULL ||
        out_document == NULL || !service->started) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    adapter = umi_workbench_layout_data_server_store_adapter(
        &service->store);
    status = umi_workbench_layout_store_load(
        &adapter, layout_id, out_document);
    umi_workbench_layout_metrics_record_load(
        &service->metrics,
        status == UMI_STATUS_OK ? sizeof(*out_document) : 0U,
        0U, status);
    return status;
}

UmiStatus umi_workbench_layout_data_service_delete_layout(
    UmiWorkbenchLayoutDataService *service,
    const char *layout_id,
    uint64_t expected_revision,
    const char *correlation_id,
    uint64_t now_ms)
{
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchLayoutStoreAdapter adapter;
    bool transaction_started = false;
    UmiStatus status = require_write(service);
    if (status != UMI_STATUS_OK || layout_id == NULL) return status;
    status = umi_workbench_layout_data_service_load_layout(
        service, layout_id, &document);
    if (status != UMI_STATUS_OK) return status;
    adapter = umi_workbench_layout_data_server_store_adapter(
        &service->store);
    status = umi_workbench_layout_data_transaction_begin(
        service->server, &transaction_started);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_store_delete(
            &adapter, layout_id, expected_revision);
    }
    if (status == UMI_STATUS_OK) {
        document.version.base_revision = expected_revision;
        document.version.revision = expected_revision + 1U;
        status = append_revision(
            service, &document, expected_revision,
            correlation_id, "Layout deleted.", now_ms, true);
    }
    if (status == UMI_STATUS_OK) {
        status = append_change_and_outbox(
            service, layout_id,
            UMI_WORKBENCH_LAYOUT_DATA_CHANGE_DELETED,
            expected_revision + 1U, expected_revision,
            document.content_hash, correlation_id,
            "Layout deleted.", now_ms);
    }
    status = umi_workbench_layout_data_transaction_finish(
        service->server, transaction_started, status);
    umi_workbench_layout_metrics_record_delete(
        &service->metrics, 0U, status);
    return status;
}

UmiStatus umi_workbench_layout_data_service_share_layout(
    UmiWorkbenchLayoutDataService *service,
    const UmiWorkbenchLayoutGrant *grant,
    const char *correlation_id,
    uint64_t now_ms)
{
    UmiStatus status = require_write(service);
    if (status != UMI_STATUS_OK || grant == NULL) return status;
    status = umi_workbench_layout_grant_store_save(
        &service->grants, grant);
    if (status == UMI_STATUS_OK) {
        status = append_change_and_outbox(
            service, grant->layout_id,
            UMI_WORKBENCH_LAYOUT_DATA_CHANGE_SHARED,
            0U, 0U, 0U, correlation_id,
            "Layout sharing grant changed.", now_ms);
    }
    return status;
}

UmiStatus umi_workbench_layout_data_service_acquire_lease(
    UmiWorkbenchLayoutDataService *service,
    const char *layout_id,
    const char *holder_id,
    const char *client_id,
    uint64_t now_ms,
    UmiWorkbenchLayoutLease *out_lease)
{
    UmiStatus status = require_write(service);
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_layout_lease_store_acquire(
        &service->leases, layout_id, holder_id,
        client_id, now_ms,
        service->config.lease_duration_ms, out_lease);
}

UmiStatus umi_workbench_layout_data_service_checkpoint_session(
    UmiWorkbenchLayoutDataService *service,
    const UmiWorkbenchLayoutSession *session,
    uint64_t expected_revision,
    uint64_t *out_revision)
{
    UmiWorkbenchLayoutStoreAdapter adapter;
    UmiStatus status = require_write(service);
    if (status != UMI_STATUS_OK || session == NULL) return status;
    adapter = umi_workbench_layout_data_server_store_adapter(
        &service->store);
    return umi_workbench_layout_store_save_session(
        &adapter, session, expected_revision, out_revision);
}

UmiStatus umi_workbench_layout_data_service_backup(
    UmiWorkbenchLayoutDataService *service,
    const char *backup_id,
    uint64_t now_ms,
    char *buffer,
    size_t capacity,
    UmiWorkbenchLayoutBackupResult *out_result)
{
    if (service == NULL || !service->started) {
        return UMI_STATUS_INVALID_STATE;
    }
    return umi_workbench_layout_backup_create(
        service->server, backup_id, service->config.replica_id,
        now_ms, NULL, buffer, capacity, out_result);
}

UmiStatus umi_workbench_layout_data_service_restore(
    UmiWorkbenchLayoutDataService *service,
    const char *backup,
    size_t length,
    const UmiWorkbenchLayoutRestoreOptions *options,
    UmiWorkbenchLayoutRestoreResult *out_result)
{
    UmiStatus status = require_write(service);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_restore_apply(
        service->server, backup, length, options, out_result);
    if (status == UMI_STATUS_OK) service->revision += 1U;
    return status;
}

UmiStatus umi_workbench_layout_data_service_snapshot(
    UmiWorkbenchLayoutDataService *service,
    uint64_t now_ms,
    UmiWorkbenchLayoutDataServiceSnapshot *out_snapshot)
{
    size_t layout_count = 0U;
    size_t session_count = 0U;
    size_t conflicts;
    size_t presence;
    UmiStatus status;
    if (service == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = sizeof(*out_snapshot);
    (void)umi_workbench_layout_data_copy_text(
        out_snapshot->replica_id, sizeof(out_snapshot->replica_id),
        service->config.replica_id, false);
    status = umi_workbench_layout_data_server_store_verify(
        &service->store, &layout_count, &session_count);
    conflicts = umi_workbench_layout_conflict_store_open_count(
        &service->conflicts);
    presence = umi_workbench_layout_presence_store_active_count(
        &service->presence, now_ms);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_metrics_snapshot(
            &service->metrics, &out_snapshot->metrics);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_health_evaluate(
            service->server, &out_snapshot->metrics,
            0U, conflicts, 0U, presence, now_ms,
            service->last_success_at_ms,
            service->last_failure_at_ms,
            &service->config.health_policy,
            &out_snapshot->health);
    }
    out_snapshot->layout_count = layout_count;
    out_snapshot->session_count = session_count;
    out_snapshot->open_conflict_count = conflicts;
    out_snapshot->active_presence_count = presence;
    out_snapshot->revision = service->revision;
    out_snapshot->started = service->started;
    out_snapshot->accepting_writes = service->accepting_writes;
    return status;
}

UmiWorkbenchLayoutStoreAdapter
umi_workbench_layout_data_service_store_adapter(
    UmiWorkbenchLayoutDataService *service)
{
    return service != NULL
        ? umi_workbench_layout_data_server_store_adapter(
              &service->store)
        : (UmiWorkbenchLayoutStoreAdapter){0};
}

UmiWorkbenchLayoutSyncEngine *
umi_workbench_layout_data_service_sync_engine(
    UmiWorkbenchLayoutDataService *service)
{
    return service != NULL ? &service->sync : NULL;
}
