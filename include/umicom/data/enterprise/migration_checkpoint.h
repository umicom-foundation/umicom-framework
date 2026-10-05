/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/migration_checkpoint.h
 *
 * PURPOSE:
 *   Record resumable migration position and pre/post schema fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_MIGRATION_CHECKPOINT_H
#define UMICOM_DATA_ENTERPRISE_MIGRATION_CHECKPOINT_H

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
 * Represent the data migration checkpoint data shared with callers of this public
 * contract.
 */
typedef struct UmiDataMigrationCheckpoint {
    char checkpoint_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char migration_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    size_t completed_steps;
    uint64_t source_fingerprint;
    uint64_t current_fingerprint;
    bool committed;
} UmiDataMigrationCheckpoint;

/* Initialise a validated migration checkpoint descriptor. */
UmiStatus umi_data_migration_checkpoint_init(UmiDataMigrationCheckpoint *item, const char *checkpoint_id, const char *migration_id, size_t completed_steps, uint64_t source_fingerprint, uint64_t current_fingerprint);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_migration_checkpoint_validate(const UmiDataMigrationCheckpoint *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_data_migration_checkpoint_archive_encode(const UmiDataMigrationCheckpoint *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_data_migration_checkpoint_archive_decode(const void *bytes, size_t byte_count,
    UmiDataMigrationCheckpoint *value);

#ifdef __cplusplus
}
#endif
#endif
