/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/migration.h
 *
 * PURPOSE:
 *   Declare ordered schema migrations with transactional apply, current-version tracking and dry-run planning.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiMigration {
    uint32_t version;
    const char *name;
    const char *up_sql;
    const char *down_sql;
} UmiMigration;

typedef struct UmiMigrationPlan {
    uint32_t current_version;
    uint32_t target_version;
    size_t pending_count;
} UmiMigrationPlan;

typedef struct UmiMigrationRegistry UmiMigrationRegistry;

UmiStatus umi_migration_registry_create(size_t capacity,
                                        UmiMigrationRegistry **out_registry);
void umi_migration_registry_destroy(UmiMigrationRegistry *registry);
UmiStatus umi_migration_registry_add(UmiMigrationRegistry *registry,
                                     const UmiMigration *migration);
UmiStatus umi_migration_plan(const UmiMigrationRegistry *registry,
                             UmiDataServer *server,
                             uint32_t target_version,
                             UmiMigrationPlan *out_plan);
UmiStatus umi_migration_apply(const UmiMigrationRegistry *registry,
                              UmiDataServer *server,
                              uint32_t target_version,
                              size_t *out_applied);

#ifdef __cplusplus
}
#endif

#endif
