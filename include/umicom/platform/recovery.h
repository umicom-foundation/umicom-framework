/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/recovery.h
 *
 * PURPOSE:
 *   Define crash-recovery storage for unsaved document content so products can
 *   restore work without placing recovery policy inside editor widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_RECOVERY_H
#define UMICOM_PLATFORM_RECOVERY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/platform/document_store.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the recovery record data shared with callers of this public contract.
 */
typedef struct UmiRecoveryRecord {
    UmiDocumentId document_id;
    uint64_t revision;
    char source_path[UMI_PATH_CAPACITY];
    char *text;
    size_t length;
} UmiRecoveryRecord;

/**
 * Represent the recovery manager data shared with callers of this public contract.
 */
typedef struct UmiRecoveryManager UmiRecoveryManager;

/**
 * Initialise recovery manager from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_recovery_manager_create(const char *root_directory,
                                      UmiRecoveryManager **out_manager);
/**
 * Release or reset state held by recovery manager so the same storage can be reused
 * safely.
 */
void umi_recovery_manager_destroy(UmiRecoveryManager *manager);
/**
 * Write recovery manager in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_recovery_manager_save(UmiRecoveryManager *manager,
                                    UmiDocumentId document_id,
                                    const char *source_path,
                                    uint64_t revision,
                                    const char *text,
                                    size_t length);
/**
 * Read recovery manager into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_recovery_manager_load(const UmiRecoveryManager *manager,
                                    UmiDocumentId document_id,
                                    UmiRecoveryRecord *out_record);
/**
 * Release or reset state held by recovery record so the same storage can be reused safely.
 */
void umi_recovery_record_dispose(UmiRecoveryRecord *record);
/**
 * Provide the recovery manager exists operation used by this module and its client
 * applications.
 */
int umi_recovery_manager_exists(const UmiRecoveryManager *manager,
                                UmiDocumentId document_id);
/**
 * Remove recovery manager while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_recovery_manager_remove(UmiRecoveryManager *manager,
                                      UmiDocumentId document_id);
/**
 * Provide the recovery manager purge operation used by this module and its client
 * applications.
 */
UmiStatus umi_recovery_manager_purge(UmiRecoveryManager *manager);
/**
 * Provide the recovery manager root operation used by this module and its client
 * applications.
 */
const char *umi_recovery_manager_root(const UmiRecoveryManager *manager);

#ifdef __cplusplus
}
#endif

#endif
