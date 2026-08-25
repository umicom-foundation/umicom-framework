/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/backup_manifest.h
 *
 * PURPOSE:
 *   Record completed backup evidence including content fingerprint and byte count.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_BACKUP_MANIFEST_H
#define UMICOM_DATA_ENTERPRISE_BACKUP_MANIFEST_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataBackupManifest {
    char backup_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    uint64_t created_at;
    uint64_t schema_fingerprint;
    uint64_t content_fingerprint;
    uint64_t bytes_written;
    bool complete;
} UmiDataBackupManifest;

/* Initialise a validated backup manifest descriptor. */
UmiStatus umi_data_backup_manifest_init(UmiDataBackupManifest *item, const char *backup_id, uint64_t created_at, uint64_t schema_fingerprint, uint64_t content_fingerprint, uint64_t bytes_written);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_backup_manifest_validate(const UmiDataBackupManifest *item);

#ifdef __cplusplus
}
#endif
#endif
