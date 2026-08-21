/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/backup.h
 *
 * PURPOSE:
 *   Export Data Server-owned layout records into a deterministic, checksummed and policy-filtered portable backup stream.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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


typedef struct UmiWorkbenchLayoutBackupOptions {
    uint32_t structure_size;
    bool include_sessions;
    bool include_audit;
    bool include_outbox;
    bool include_presence;
    bool include_migrations;
} UmiWorkbenchLayoutBackupOptions;

typedef struct UmiWorkbenchLayoutBackupResult {
    uint32_t structure_size;
    UmiStatus status;
    UmiWorkbenchLayoutBackupManifest manifest;
    size_t bytes_required;
    size_t bytes_written;
    char message[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY];
} UmiWorkbenchLayoutBackupResult;

UmiWorkbenchLayoutBackupOptions
umi_workbench_layout_backup_options_default(void);

UmiStatus umi_workbench_layout_backup_create(
    const UmiDataServer *server,
    const char *backup_id,
    const char *replica_id,
    uint64_t created_at_ms,
    const UmiWorkbenchLayoutBackupOptions *options,
    char *buffer,
    size_t capacity,
    UmiWorkbenchLayoutBackupResult *out_result);

UmiStatus umi_workbench_layout_backup_validate(
    const char *backup,
    size_t length,
    UmiWorkbenchLayoutBackupManifest *out_manifest);

#ifdef __cplusplus
}
#endif

#endif
