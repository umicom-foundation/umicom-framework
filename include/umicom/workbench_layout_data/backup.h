/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/backup.h
 *
 * PURPOSE:
 *   Export Data Server-owned layout records into a deterministic, checksummed and policy-filtered portable backup stream.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_BACKUP_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_BACKUP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench layout backup options data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutBackupOptions {
    uint32_t structure_size;
    bool include_sessions;
    bool include_audit;
    bool include_outbox;
    bool include_presence;
    bool include_migrations;
} UmiWorkbenchLayoutBackupOptions;

/**
 * Represent the workbench layout backup result data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutBackupResult {
    uint32_t structure_size;
    UmiStatus status;
    UmiWorkbenchLayoutBackupManifest manifest;
    size_t bytes_required;
    size_t bytes_written;
    char message[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY];
} UmiWorkbenchLayoutBackupResult;

/**
 * Provide the workbench layout backup options default operation used by this module and
 * its client applications.
 */
UmiWorkbenchLayoutBackupOptions
umi_workbench_layout_backup_options_default(void);

/**
 * Initialise workbench layout backup from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_workbench_layout_backup_create(
    const UmiDataServer *server,
    const char *backup_id,
    const char *replica_id,
    uint64_t created_at_ms,
    const UmiWorkbenchLayoutBackupOptions *options,
    char *buffer,
    size_t capacity,
    UmiWorkbenchLayoutBackupResult *out_result);

/**
 * Check that workbench layout backup satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_workbench_layout_backup_validate(
    const char *backup,
    size_t length,
    UmiWorkbenchLayoutBackupManifest *out_manifest);

#ifdef __cplusplus
}
#endif

#endif
