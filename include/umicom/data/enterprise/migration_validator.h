/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/migration_validator.h
 *
 * PURPOSE:
 *   Apply destructive/reversibility/schema-compatibility policy before a migration plan can execute.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_MIGRATION_VALIDATOR_H
#define UMICOM_DATA_ENTERPRISE_MIGRATION_VALIDATOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/migration_plan.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data migration validation data shared with callers of this public
 * contract.
 */
typedef struct UmiDataMigrationValidation { bool valid; bool requires_backup; bool requires_approval; size_t blockers; } UmiDataMigrationValidation;
/* Validate a migration plan against compatibility and operational policy. */ UmiStatus umi_data_migration_validator_check(const UmiDataMigrationPlan *plan,UmiDataCompatibility compatibility,bool backup_available,bool destructive_approved,UmiDataMigrationValidation *out_validation);

#ifdef __cplusplus
}
#endif
#endif
