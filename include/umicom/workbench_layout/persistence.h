/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/persistence.h
 *
 * PURPOSE:
 *   Coordinate layout and session persistence through one authoritative store adapter with validation, optimistic revisions and transactions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_PERSISTENCE_H
#define UMICOM_WORKBENCH_LAYOUT_PERSISTENCE_H

#include "umicom/workbench_layout/recovery.h"
#include "umicom/workbench_layout/store.h"
#include "umicom/workbench_layout/validation.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchLayoutPersistenceConfig {
    uint32_t structure_size;
    bool validate_before_save;
    bool require_transactions_for_session_checkpoint;
    bool update_modified_audit;
    bool refresh_content_hash;
} UmiWorkbenchLayoutPersistenceConfig;

typedef struct UmiWorkbenchLayoutPersistenceResult {
    uint32_t structure_size;
    UmiStatus status;
    uint64_t previous_revision;
    uint64_t resulting_revision;
    UmiWorkbenchLayoutValidationReport validation;
    bool transaction_used;
    bool recovery_checkpoint_written;
    char message[UMI_WORKBENCH_LAYOUT_ERROR_CAPACITY];
} UmiWorkbenchLayoutPersistenceResult;

typedef struct UmiWorkbenchLayoutPersistenceService
    UmiWorkbenchLayoutPersistenceService;

UmiWorkbenchLayoutPersistenceConfig
umi_workbench_layout_persistence_config_default(void);

UmiStatus umi_workbench_layout_persistence_create(
    const UmiWorkbenchLayoutStoreAdapter *adapter,
    const UmiWorkbenchLayoutPersistenceConfig *config,
    UmiWorkbenchRecoveryJournal *recovery_journal,
    UmiWorkbenchLayoutPersistenceService **out_service);

void umi_workbench_layout_persistence_destroy(
    UmiWorkbenchLayoutPersistenceService *service);

UmiStatus umi_workbench_layout_persistence_save_layout(
    UmiWorkbenchLayoutPersistenceService *service,
    UmiWorkbenchLayoutDocument *document,
    uint64_t expected_revision,
    const char *actor_id,
    const char *correlation_id,
    uint64_t timestamp_ms,
    UmiWorkbenchLayoutPersistenceResult *out_result);

UmiStatus umi_workbench_layout_persistence_load_layout(
    UmiWorkbenchLayoutPersistenceService *service,
    const char *layout_id,
    UmiWorkbenchLayoutDocument *out_document,
    UmiWorkbenchLayoutPersistenceResult *out_result);

UmiStatus umi_workbench_layout_persistence_delete_layout(
    UmiWorkbenchLayoutPersistenceService *service,
    const char *layout_id,
    uint64_t expected_revision,
    UmiWorkbenchLayoutPersistenceResult *out_result);

UmiStatus umi_workbench_layout_persistence_checkpoint_session(
    UmiWorkbenchLayoutPersistenceService *service,
    UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutSession *session,
    const char *actor_id,
    const char *correlation_id,
    uint64_t timestamp_ms,
    UmiWorkbenchLayoutPersistenceResult *out_result);

UmiStatus umi_workbench_layout_persistence_restore_session(
    UmiWorkbenchLayoutPersistenceService *service,
    const char *session_id,
    UmiWorkbenchLayoutSession *out_session,
    UmiWorkbenchLayoutDocument *out_document,
    UmiWorkbenchLayoutPersistenceResult *out_result);

const UmiWorkbenchLayoutStoreAdapter *
umi_workbench_layout_persistence_adapter(
    const UmiWorkbenchLayoutPersistenceService *service);

#ifdef __cplusplus
}
#endif

#endif
