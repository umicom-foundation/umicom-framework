/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/migration.c
 *
 * PURPOSE:
 *   Implement ordered migration registration, planning and transactional execution with schema-version updates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Provide the copy text operation used by this module and its client applications. */
static char *copy_text(const char *text)
{
    size_t length;
    char *copy;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) text = "";
    length = strlen(text);
    copy = (char *)malloc(length + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (copy != NULL) (void)memcpy(copy, text, length + 1U);
    return copy;
}

/*
 * Initialise migration registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_migration_registry_create(size_t capacity,
                                        UmiMigrationRegistry **out_registry)
{
    UmiMigrationRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (capacity == 0U || out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiMigrationRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->migrations = (UmiOwnedMigration *)calloc(capacity,
                                                        sizeof(*registry->migrations));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry->migrations == NULL) {
        free(registry);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    registry->capacity = capacity;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by migration registry so the same storage can be reused
 * safely.
 */
void umi_migration_registry_destroy(UmiMigrationRegistry *registry)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        free(registry->migrations[index].name);
        free(registry->migrations[index].up_sql);
        free(registry->migrations[index].down_sql);
    }
    free(registry->migrations);
    free(registry);
}

/* Add migration registry only after its inputs and available capacity have been checked. */
UmiStatus umi_migration_registry_add(UmiMigrationRegistry *registry,
                                     const UmiMigration *migration)
{
    size_t index;
    size_t position = 0U;
    UmiOwnedMigration entry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || migration == NULL || migration->version == 0U ||
        migration->name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->migrations[index].migration.version == migration->version) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->migrations[index].migration.version < migration->version) {
            position = index + 1U;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= registry->capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memset(&entry, 0, sizeof(entry));
    entry.name = copy_text(migration->name);
    entry.up_sql = copy_text(migration->up_sql);
    entry.down_sql = copy_text(migration->down_sql);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position < registry->count) {
        (void)memmove(&registry->migrations[position + 1U],
                      &registry->migrations[position],
                      (registry->count - position) * sizeof(entry));
    }
    registry->migrations[position] = entry;
    registry->count++;
    return UMI_STATUS_OK;
}

/* Provide the current version operation used by this module and its client applications. */
static UmiStatus current_version(UmiDataServer *server, uint32_t *out_version)
{
    UmiStore store;
    UmiStatus status = umi_store_from_data_server(server, &store);
    return status == UMI_STATUS_OK
        ? umi_schema_version_get(&store, out_version)
        : status;
}

/* Provide the migration plan operation used by this module and its client applications. */
UmiStatus umi_migration_plan(const UmiMigrationRegistry *registry,
                             UmiDataServer *server,
                             uint32_t target_version,
                             UmiMigrationPlan *out_plan)
{
    size_t index;
    uint32_t current;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || server == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = current_version(server, &current);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_plan->current_version = current;
    out_plan->target_version = target_version;
    out_plan->pending_count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        uint32_t version = registry->migrations[index].migration.version;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (version > current && version <= target_version) {
            out_plan->pending_count++;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Perform migration through the module contract so client applications do not duplicate
 * its policy.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = current_version(server, &current);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_data_transaction_begin(server, &transaction);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_store_from_data_server(server, &store);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_data_transaction_dispose(&transaction);
        return status;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        const UmiMigration *migration = &registry->migrations[index].migration;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (migration->version <= current || migration->version > target_version) {
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_data_server_backend(server) == UMI_DATA_BACKEND_SQLITE &&
            migration->up_sql != NULL && migration->up_sql[0] != '\0') {
            status = umi_data_server_execute(server, migration->up_sql);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) break;
        }
        status = umi_schema_version_set(&store, migration->version);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        current = migration->version;
        applied++;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_data_transaction_commit(&transaction);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)umi_data_transaction_rollback(&transaction);
    }
    umi_data_transaction_dispose(&transaction);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_applied != NULL) *out_applied = applied;
    return status;
}
