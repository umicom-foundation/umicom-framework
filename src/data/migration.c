/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/migration.c
 *
 * PURPOSE:
 *   Implement ordered migration registration, planning and transactional execution with schema-version updates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/migration.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/data/schema_version.h"
#include "umicom/data/store.h"
#include "umicom/data/transaction.h"

typedef struct UmiOwnedMigration {
    UmiMigration migration;
    char *name;
    char *up_sql;
    char *down_sql;
} UmiOwnedMigration;

struct UmiMigrationRegistry {
    UmiOwnedMigration *migrations;
    size_t capacity;
    size_t count;
};

static char *copy_text(const char *text)
{
    size_t length;
    char *copy;
    if (text == NULL) text = "";
    length = strlen(text);
    copy = (char *)malloc(length + 1U);
    if (copy != NULL) (void)memcpy(copy, text, length + 1U);
    return copy;
}

UmiStatus umi_migration_registry_create(size_t capacity,
                                        UmiMigrationRegistry **out_registry)
{
    UmiMigrationRegistry *registry;
    if (capacity == 0U || out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiMigrationRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->migrations = (UmiOwnedMigration *)calloc(capacity,
                                                        sizeof(*registry->migrations));
    if (registry->migrations == NULL) {
        free(registry);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    registry->capacity = capacity;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_migration_registry_destroy(UmiMigrationRegistry *registry)
{
    size_t index;
    if (registry == NULL) return;
    for (index = 0U; index < registry->count; ++index) {
        free(registry->migrations[index].name);
        free(registry->migrations[index].up_sql);
        free(registry->migrations[index].down_sql);
    }
    free(registry->migrations);
    free(registry);
}

UmiStatus umi_migration_registry_add(UmiMigrationRegistry *registry,
                                     const UmiMigration *migration)
{
    size_t index;
    size_t position = 0U;
    UmiOwnedMigration entry;
    if (registry == NULL || migration == NULL || migration->version == 0U ||
        migration->name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (registry->migrations[index].migration.version == migration->version) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
        if (registry->migrations[index].migration.version < migration->version) {
            position = index + 1U;
        }
    }
    if (registry->count >= registry->capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memset(&entry, 0, sizeof(entry));
    entry.name = copy_text(migration->name);
    entry.up_sql = copy_text(migration->up_sql);
    entry.down_sql = copy_text(migration->down_sql);
    if (entry.name == NULL || entry.up_sql == NULL || entry.down_sql == NULL) {
        free(entry.name);
        free(entry.up_sql);
        free(entry.down_sql);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    entry.migration = *migration;
    entry.migration.name = entry.name;
    entry.migration.up_sql = entry.up_sql;
    entry.migration.down_sql = entry.down_sql;
    if (position < registry->count) {
        (void)memmove(&registry->migrations[position + 1U],
                      &registry->migrations[position],
                      (registry->count - position) * sizeof(entry));
    }
    registry->migrations[position] = entry;
    registry->count++;
    return UMI_STATUS_OK;
}

static UmiStatus current_version(UmiDataServer *server, uint32_t *out_version)
{
    UmiStore store;
    UmiStatus status = umi_store_from_data_server(server, &store);
    return status == UMI_STATUS_OK
        ? umi_schema_version_get(&store, out_version)
        : status;
}

UmiStatus umi_migration_plan(const UmiMigrationRegistry *registry,
                             UmiDataServer *server,
                             uint32_t target_version,
                             UmiMigrationPlan *out_plan)
{
    size_t index;
    uint32_t current;
    UmiStatus status;
    if (registry == NULL || server == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = current_version(server, &current);
    if (status != UMI_STATUS_OK) return status;
    out_plan->current_version = current;
    out_plan->target_version = target_version;
    out_plan->pending_count = 0U;
    for (index = 0U; index < registry->count; ++index) {
        uint32_t version = registry->migrations[index].migration.version;
        if (version > current && version <= target_version) {
            out_plan->pending_count++;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_migration_apply(const UmiMigrationRegistry *registry,
                              UmiDataServer *server,
                              uint32_t target_version,
                              size_t *out_applied)
{
    UmiDataTransaction transaction;
    UmiStore store;
    UmiStatus status;
    uint32_t current;
    size_t index;
    size_t applied = 0U;
    if (registry == NULL || server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = current_version(server, &current);
    if (status != UMI_STATUS_OK) return status;
    status = umi_data_transaction_begin(server, &transaction);
    if (status != UMI_STATUS_OK) return status;
    status = umi_store_from_data_server(server, &store);
    if (status != UMI_STATUS_OK) {
        umi_data_transaction_dispose(&transaction);
        return status;
    }
    for (index = 0U; index < registry->count; ++index) {
        const UmiMigration *migration = &registry->migrations[index].migration;
        if (migration->version <= current || migration->version > target_version) {
            continue;
        }
        if (umi_data_server_backend(server) == UMI_DATA_BACKEND_SQLITE &&
            migration->up_sql != NULL && migration->up_sql[0] != '\0') {
            status = umi_data_server_execute(server, migration->up_sql);
            if (status != UMI_STATUS_OK) break;
        }
        status = umi_schema_version_set(&store, migration->version);
        if (status != UMI_STATUS_OK) break;
        current = migration->version;
        applied++;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_data_transaction_commit(&transaction);
    }
    if (status != UMI_STATUS_OK) {
        (void)umi_data_transaction_rollback(&transaction);
    }
    umi_data_transaction_dispose(&transaction);
    if (out_applied != NULL) *out_applied = applied;
    return status;
}
