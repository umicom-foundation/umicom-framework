/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/backup_plan.h
 *
 * PURPOSE:
 *   Describe a reviewable full/incremental backup request and retention class.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_BACKUP_PLAN_H
#define UMICOM_DATA_ENTERPRISE_BACKUP_PLAN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
#endif
