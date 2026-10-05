/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/backup_manifest.h
 *
 * PURPOSE:
 *   Record completed backup evidence including content fingerprint and byte count.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_BACKUP_MANIFEST_H
#define UMICOM_DATA_ENTERPRISE_BACKUP_MANIFEST_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data backup manifest data shared with callers of this public contract.
 */
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

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_data_backup_manifest_archive_encode(const UmiDataBackupManifest *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_data_backup_manifest_archive_decode(const void *bytes, size_t byte_count,
    UmiDataBackupManifest *value);

#ifdef __cplusplus
}
#endif
#endif
