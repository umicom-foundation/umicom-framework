/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/service.h
 *
 * PURPOSE:
 *   Coordinate authoritative layout persistence, revisions, sharing, leases, sessions, outbox, backups, health and synchronisation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_SERVICE_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_SERVICE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/workbench_layout_data/backup.h"
#include "umicom/workbench_layout_data/change_feed.h"
#include "umicom/workbench_layout_data/data_server_store.h"
#include "umicom/workbench_layout_data/grant_store.h"
#include "umicom/workbench_layout_data/health.h"
#include "umicom/workbench_layout_data/lease_store.h"
#include "umicom/workbench_layout_data/metrics.h"
#include "umicom/workbench_layout_data/outbox.h"
#include "umicom/workbench_layout_data/presence_store.h"
#include "umicom/workbench_layout_data/restore.h"
#include "umicom/workbench_layout_data/revision_store.h"
#include "umicom/workbench_layout_data/sync_engine.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchLayoutDataServiceConfig {
    uint32_t structure_size;
    char replica_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char actor_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    uint64_t lease_duration_ms;
    uint64_t presence_ttl_ms;
    uint64_t outbox_lease_ms;
    uint64_t offline_claim_ms;
    UmiWorkbenchLayoutSyncPolicy sync_policy;
    UmiWorkbenchLayoutHealthPolicy health_policy;
} UmiWorkbenchLayoutDataServiceConfig;

typedef struct UmiWorkbenchLayoutDataServiceSnapshot {
    uint32_t structure_size;
    char replica_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    size_t layout_count;
    size_t session_count;
    size_t open_conflict_count;
    size_t active_presence_count;
    UmiWorkbenchLayoutDataMetrics metrics;
    UmiWorkbenchLayoutDataHealthSnapshot health;
    uint64_t revision;
    bool started;
    bool accepting_writes;
} UmiWorkbenchLayoutDataServiceSnapshot;

typedef struct UmiWorkbenchLayoutDataService
    UmiWorkbenchLayoutDataService;

UmiWorkbenchLayoutDataServiceConfig
umi_workbench_layout_data_service_config_default(void);

UmiStatus umi_workbench_layout_data_service_create(
    UmiDataServer *server,
    const UmiWorkbenchLayoutDataServiceConfig *config,
    UmiWorkbenchLayoutDataService **out_service);

void umi_workbench_layout_data_service_destroy(
    UmiWorkbenchLayoutDataService *service);

UmiStatus umi_workbench_layout_data_service_start(
    UmiWorkbenchLayoutDataService *service,
    uint64_t now_ms);

UmiStatus umi_workbench_layout_data_service_quiesce(
    UmiWorkbenchLayoutDataService *service);

UmiStatus umi_workbench_layout_data_service_stop(
    UmiWorkbenchLayoutDataService *service,
    uint64_t now_ms);

UmiStatus umi_workbench_layout_data_service_save_layout(
    UmiWorkbenchLayoutDataService *service,
    UmiWorkbenchLayoutDocument *document,
    uint64_t expected_revision,
    const char *correlation_id,
    uint64_t now_ms,
    uint64_t *out_revision);

UmiStatus umi_workbench_layout_data_service_load_layout(
    UmiWorkbenchLayoutDataService *service,
    const char *layout_id,
    UmiWorkbenchLayoutDocument *out_document);

UmiStatus umi_workbench_layout_data_service_delete_layout(
    UmiWorkbenchLayoutDataService *service,
    const char *layout_id,
    uint64_t expected_revision,
    const char *correlation_id,
    uint64_t now_ms);

UmiStatus umi_workbench_layout_data_service_share_layout(
    UmiWorkbenchLayoutDataService *service,
    const UmiWorkbenchLayoutGrant *grant,
    const char *correlation_id,
    uint64_t now_ms);

UmiStatus umi_workbench_layout_data_service_acquire_lease(
    UmiWorkbenchLayoutDataService *service,
    const char *layout_id,
    const char *holder_id,
    const char *client_id,
    uint64_t now_ms,
    UmiWorkbenchLayoutLease *out_lease);

UmiStatus umi_workbench_layout_data_service_checkpoint_session(
    UmiWorkbenchLayoutDataService *service,
    const UmiWorkbenchLayoutSession *session,
    uint64_t expected_revision,
    uint64_t *out_revision);

UmiStatus umi_workbench_layout_data_service_backup(
    UmiWorkbenchLayoutDataService *service,
    const char *backup_id,
    uint64_t now_ms,
    char *buffer,
    size_t capacity,
    UmiWorkbenchLayoutBackupResult *out_result);

UmiStatus umi_workbench_layout_data_service_restore(
    UmiWorkbenchLayoutDataService *service,
    const char *backup,
    size_t length,
    const UmiWorkbenchLayoutRestoreOptions *options,
    UmiWorkbenchLayoutRestoreResult *out_result);

UmiStatus umi_workbench_layout_data_service_snapshot(
    UmiWorkbenchLayoutDataService *service,
    uint64_t now_ms,
    UmiWorkbenchLayoutDataServiceSnapshot *out_snapshot);

UmiWorkbenchLayoutStoreAdapter
umi_workbench_layout_data_service_store_adapter(
    UmiWorkbenchLayoutDataService *service);

UmiWorkbenchLayoutSyncEngine *
umi_workbench_layout_data_service_sync_engine(
    UmiWorkbenchLayoutDataService *service);

#ifdef __cplusplus
}
#endif

#endif
