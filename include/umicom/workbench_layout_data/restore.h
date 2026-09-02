/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/restore.h
 *
 * PURPOSE:
 *   Restore portable layout backups through one transaction with record filtering, validation-only mode and explicit evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_RESTORE_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_RESTORE_H

#include <stdbool.h>
#include <stddef.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/backup.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench layout restore options data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutRestoreOptions {
    uint32_t structure_size;
    bool replace_existing;
    bool restore_sessions;
    bool restore_outbox;
    bool restore_presence;
    bool validate_only;
} UmiWorkbenchLayoutRestoreOptions;

/**
 * Represent the workbench layout restore result data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutRestoreResult {
    uint32_t structure_size;
    UmiStatus status;
    size_t parsed_count;
    size_t restored_count;
    size_t skipped_count;
    size_t failed_count;
    char failed_key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char message[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY];
} UmiWorkbenchLayoutRestoreResult;

/**
 * Provide the workbench layout restore options default operation used by this module and
 * its client applications.
 */
UmiWorkbenchLayoutRestoreOptions
umi_workbench_layout_restore_options_default(void);

/**
 * Perform workbench layout restore through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_workbench_layout_restore_apply(
    UmiDataServer *server,
    const char *backup,
    size_t length,
    const UmiWorkbenchLayoutRestoreOptions *options,
    UmiWorkbenchLayoutRestoreResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
