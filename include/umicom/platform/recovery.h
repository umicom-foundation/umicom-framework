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

typedef struct UmiRecoveryRecord {
    UmiDocumentId document_id;
    uint64_t revision;
    char source_path[UMI_PATH_CAPACITY];
    char *text;
    size_t length;
} UmiRecoveryRecord;

typedef struct UmiRecoveryManager UmiRecoveryManager;

UmiStatus umi_recovery_manager_create(const char *root_directory,
                                      UmiRecoveryManager **out_manager);
void umi_recovery_manager_destroy(UmiRecoveryManager *manager);
UmiStatus umi_recovery_manager_save(UmiRecoveryManager *manager,
                                    UmiDocumentId document_id,
                                    const char *source_path,
                                    uint64_t revision,
                                    const char *text,
                                    size_t length);
UmiStatus umi_recovery_manager_load(const UmiRecoveryManager *manager,
                                    UmiDocumentId document_id,
                                    UmiRecoveryRecord *out_record);
void umi_recovery_record_dispose(UmiRecoveryRecord *record);
int umi_recovery_manager_exists(const UmiRecoveryManager *manager,
                                UmiDocumentId document_id);
UmiStatus umi_recovery_manager_remove(UmiRecoveryManager *manager,
                                      UmiDocumentId document_id);
UmiStatus umi_recovery_manager_purge(UmiRecoveryManager *manager);
const char *umi_recovery_manager_root(const UmiRecoveryManager *manager);

#ifdef __cplusplus
}
#endif

#endif
