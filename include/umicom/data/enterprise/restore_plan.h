/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/restore_plan.h
 *
 * PURPOSE:
 *   Describe a restore target and safety gates before destructive data replacement.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_RESTORE_PLAN_H
#define UMICOM_DATA_ENTERPRISE_RESTORE_PLAN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataRestorePlan {
    char restore_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char backup_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    uint64_t expected_schema_fingerprint;
    bool verify_only;
    bool preserve_existing;
    bool approved;
} UmiDataRestorePlan;

/* Initialise a validated restore plan descriptor. */
UmiStatus umi_data_restore_plan_init(UmiDataRestorePlan *item, const char *restore_id, const char *backup_id, uint64_t expected_schema_fingerprint, bool verify_only);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_restore_plan_validate(const UmiDataRestorePlan *item);

#ifdef __cplusplus
}
#endif
#endif
