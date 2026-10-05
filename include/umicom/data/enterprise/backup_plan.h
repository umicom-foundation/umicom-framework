/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/backup_plan.h
 *
 * PURPOSE:
 *   Describe a reviewable full/incremental backup request and retention class.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_BACKUP_PLAN_H
#define UMICOM_DATA_ENTERPRISE_BACKUP_PLAN_H

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
 * Represent the data backup plan data shared with callers of this public contract.
 */
typedef struct UmiDataBackupPlan {
    char backup_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char destination[UMI_DATA_ENTERPRISE_PATH_CAPACITY];
    uint64_t schema_fingerprint;
    bool incremental;
    bool include_blobs;
    bool encrypted;
} UmiDataBackupPlan;

/* Initialise a validated backup plan descriptor. */
UmiStatus umi_data_backup_plan_init(UmiDataBackupPlan *item, const char *backup_id, const char *destination, uint64_t schema_fingerprint, bool incremental);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_backup_plan_validate(const UmiDataBackupPlan *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_data_backup_plan_archive_encode(const UmiDataBackupPlan *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_data_backup_plan_archive_decode(const void *bytes, size_t byte_count,
    UmiDataBackupPlan *value);

#ifdef __cplusplus
}
#endif
#endif
