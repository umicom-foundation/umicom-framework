/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/migration.h
 *
 * PURPOSE:
 *   Declare ordered schema migrations with transactional apply, current-version tracking and dry-run planning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_MIGRATION_H
#define UMICOM_DATA_MIGRATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/data/data_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the migration data shared with callers of this public contract.
 */
typedef struct UmiMigration {
    uint32_t version;
    const char *name;
    const char *up_sql;
    const char *down_sql;
} UmiMigration;

/**
 * Represent the migration plan data shared with callers of this public contract.
 */
typedef struct UmiMigrationPlan {
    uint32_t current_version;
    uint32_t target_version;
    size_t pending_count;
} UmiMigrationPlan;

/**
 * Represent the migration registry data shared with callers of this public contract.
 */
typedef struct UmiMigrationRegistry UmiMigrationRegistry;

/**
 * Initialise migration registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_migration_registry_create(size_t capacity,
                                        UmiMigrationRegistry **out_registry);
/**
 * Release or reset state held by migration registry so the same storage can be reused
 * safely.
 */
void umi_migration_registry_destroy(UmiMigrationRegistry *registry);
/**
 * Add migration registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_migration_registry_add(UmiMigrationRegistry *registry,
                                     const UmiMigration *migration);
/**
 * Provide the migration plan operation used by this module and its client applications.
 */
UmiStatus umi_migration_plan(const UmiMigrationRegistry *registry,
                             UmiDataServer *server,
                             uint32_t target_version,
                             UmiMigrationPlan *out_plan);
/**
 * Perform migration through the module contract so client applications do not duplicate
 * its policy.
 */
UmiStatus umi_migration_apply(const UmiMigrationRegistry *registry,
                              UmiDataServer *server,
                              uint32_t target_version,
                              size_t *out_applied);

#ifdef __cplusplus
}
#endif

#endif
