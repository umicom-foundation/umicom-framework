/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/migration_runner.h
 *
 * PURPOSE:
 *   Apply restartable, transactionally recorded layout persistence schema migrations through the Data Server authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_MIGRATION_RUNNER_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_MIGRATION_RUNNER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/migration_store.h"
#include "umicom/workbench_layout_data/schema_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef UmiStatus (*UmiWorkbenchLayoutMigrationApplyFunction)(
    UmiDataServer *server,
    void *context);

typedef struct UmiWorkbenchLayoutDataMigrationStep {
    uint32_t structure_size;
    UmiWorkbenchLayoutMigrationDescriptor descriptor;
    UmiWorkbenchLayoutMigrationApplyFunction apply;
    UmiWorkbenchLayoutMigrationApplyFunction rollback;
    void *context;
} UmiWorkbenchLayoutDataMigrationStep;

typedef struct UmiWorkbenchLayoutDataMigrationPlan {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataMigrationStep
        steps[UMI_WORKBENCH_LAYOUT_DATA_MAX_MIGRATIONS];
    size_t count;
    uint32_t source_version;
    uint32_t target_version;
} UmiWorkbenchLayoutDataMigrationPlan;

typedef struct UmiWorkbenchLayoutMigrationResult {
    uint32_t structure_size;
    UmiStatus status;
    size_t applied_count;
    size_t skipped_count;
    size_t rollback_count;
    uint32_t initial_version;
    uint32_t resulting_version;
    char failed_migration_id[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char message[UMI_WORKBENCH_LAYOUT_DATA_MESSAGE_CAPACITY];
} UmiWorkbenchLayoutMigrationResult;

UmiStatus umi_workbench_layout_migration_plan_init(
    UmiWorkbenchLayoutDataMigrationPlan *plan,
    uint32_t source_version,
    uint32_t target_version);

UmiStatus umi_workbench_layout_migration_plan_add(
    UmiWorkbenchLayoutDataMigrationPlan *plan,
    const UmiWorkbenchLayoutDataMigrationStep *step);

UmiStatus umi_workbench_layout_migration_read_version(
    const UmiDataServer *server,
    uint32_t *out_version);

UmiStatus umi_workbench_layout_migration_write_version(
    UmiDataServer *server,
    uint32_t version);

UmiStatus umi_workbench_layout_migration_execute(
    UmiDataServer *server,
    UmiWorkbenchLayoutMigrationStoreRepository *records,
    const UmiWorkbenchLayoutDataMigrationPlan *plan,
    const char *actor_id,
    uint64_t now_ms,
    UmiWorkbenchLayoutMigrationResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
