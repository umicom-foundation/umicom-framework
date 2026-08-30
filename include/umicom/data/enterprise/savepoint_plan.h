/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/savepoint_plan.h
 *
 * PURPOSE:
 *   Describe explicit savepoints for backend adapters that support nested recovery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_SAVEPOINT_PLAN_H
#define UMICOM_DATA_ENTERPRISE_SAVEPOINT_PLAN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataSavepointPlan {
    char savepoint_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    uint32_t ordinal;
    bool release_on_success;
    bool rollback_on_failure;
} UmiDataSavepointPlan;

/* Initialise a validated savepoint plan descriptor. */
UmiStatus umi_data_savepoint_plan_init(UmiDataSavepointPlan *item, const char *savepoint_id, uint32_t ordinal);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_savepoint_plan_validate(const UmiDataSavepointPlan *item);

#ifdef __cplusplus
}
#endif
#endif
