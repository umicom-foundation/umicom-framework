/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/persistence.h
 *
 * PURPOSE:
 *   Coordinate layout and session persistence through one authoritative store adapter with validation, optimistic revisions and transactions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_PERSISTENCE_H
#define UMICOM_WORKBENCH_LAYOUT_PERSISTENCE_H

#include "umicom/workbench_layout/recovery.h"
#include "umicom/workbench_layout/store.h"
#include "umicom/workbench_layout/validation.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench layout persistence config data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutPersistenceConfig {
    uint32_t structure_size;
    bool validate_before_save;
    bool require_transactions_for_session_checkpoint;
    bool update_modified_audit;
    bool refresh_content_hash;
} UmiWorkbenchLayoutPersistenceConfig;

/**
 * Represent the workbench layout persistence result data shared with callers of this
 * public contract.
 */
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

/**
 * Represent the workbench layout persistence service data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutPersistenceService
    UmiWorkbenchLayoutPersistenceService;

/**
 * Provide the workbench layout persistence config default operation used by this module
 * and its client applications.
 */
UmiWorkbenchLayoutPersistenceConfig
umi_workbench_layout_persistence_config_default(void);

/**
 * Initialise workbench layout persistence from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_workbench_layout_persistence_create(
    const UmiWorkbenchLayoutStoreAdapter *adapter,
    const UmiWorkbenchLayoutPersistenceConfig *config,
    UmiWorkbenchRecoveryJournal *recovery_journal,
    UmiWorkbenchLayoutPersistenceService **out_service);

/**
 * Release or reset state held by workbench layout persistence so the same storage can be
 * reused safely.
 */
void umi_workbench_layout_persistence_destroy(
    UmiWorkbenchLayoutPersistenceService *service);

/**
 * Provide the workbench layout persistence save layout operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_persistence_save_layout(
    UmiWorkbenchLayoutPersistenceService *service,
    UmiWorkbenchLayoutDocument *document,
    uint64_t expected_revision,
    const char *actor_id,
    const char *correlation_id,
    uint64_t timestamp_ms,
    UmiWorkbenchLayoutPersistenceResult *out_result);

/**
 * Provide the workbench layout persistence load layout operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_persistence_load_layout(
    UmiWorkbenchLayoutPersistenceService *service,
    const char *layout_id,
    UmiWorkbenchLayoutDocument *out_document,
    UmiWorkbenchLayoutPersistenceResult *out_result);

/**
 * Provide the workbench layout persistence delete layout operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_persistence_delete_layout(
    UmiWorkbenchLayoutPersistenceService *service,
    const char *layout_id,
    uint64_t expected_revision,
    UmiWorkbenchLayoutPersistenceResult *out_result);

/**
 * Provide the workbench layout persistence checkpoint session operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_layout_persistence_checkpoint_session(
    UmiWorkbenchLayoutPersistenceService *service,
    UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutSession *session,
    const char *actor_id,
    const char *correlation_id,
    uint64_t timestamp_ms,
    UmiWorkbenchLayoutPersistenceResult *out_result);

/**
 * Provide the workbench layout persistence restore session operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_persistence_restore_session(
    UmiWorkbenchLayoutPersistenceService *service,
    const char *session_id,
    UmiWorkbenchLayoutSession *out_session,
    UmiWorkbenchLayoutDocument *out_document,
    UmiWorkbenchLayoutPersistenceResult *out_result);

/**
 * Provide the workbench layout persistence adapter operation used by this module and its
 * client applications.
 */
const UmiWorkbenchLayoutStoreAdapter *
umi_workbench_layout_persistence_adapter(
    const UmiWorkbenchLayoutPersistenceService *service);

#ifdef __cplusplus
}
#endif

#endif
