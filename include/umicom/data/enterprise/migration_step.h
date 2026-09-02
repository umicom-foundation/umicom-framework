/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/migration_step.h
 *
 * PURPOSE:
 *   Describe one reversible or irreversible schema/data migration operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_MIGRATION_STEP_H
#define UMICOM_DATA_ENTERPRISE_MIGRATION_STEP_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data migration step data shared with callers of this public contract.
 */
typedef struct UmiDataMigrationStep {
    char step_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char description[UMI_DATA_ENTERPRISE_TEXT_CAPACITY];
    uint32_t ordinal;
    bool reversible;
    bool destructive;
} UmiDataMigrationStep;

/* Initialise a validated migration step descriptor. */
UmiStatus umi_data_migration_step_init(UmiDataMigrationStep *item, const char *step_id, const char *description, uint32_t ordinal, bool reversible, bool destructive);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_migration_step_validate(const UmiDataMigrationStep *item);

#ifdef __cplusplus
}
#endif
#endif
