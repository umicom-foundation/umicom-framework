/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/migration_checkpoint.h
 *
 * PURPOSE:
 *   Record resumable migration position and pre/post schema fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_MIGRATION_CHECKPOINT_H
#define UMICOM_DATA_ENTERPRISE_MIGRATION_CHECKPOINT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
#endif
