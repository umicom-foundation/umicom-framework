/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/migration_runner.c
 *
 * PURPOSE:
 *   Implement restartable schema-version management, transaction boundaries and migration evidence recording.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/migration_runner.h"
#include <stdio.h>
#include <string.h>
#include "internal.h"



#define UMI_LAYOUT_DATA_SCHEMA_VERSION_KEY \
    "workbench-layout/metadata/schema-version"

UmiStatus umi_workbench_layout_migration_plan_init(
    UmiWorkbenchLayoutDataMigrationPlan *plan,
    uint32_t source_version,
    uint32_t target_version)
{
    if (plan == NULL || source_version > target_version) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(plan, 0, sizeof(*plan));
    plan->structure_size = sizeof(*plan);
    plan->source_version = source_version;
    plan->target_version = target_version;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_migration_plan_add(
    UmiWorkbenchLayoutDataMigrationPlan *plan,
    const UmiWorkbenchLayoutDataMigrationStep *step)
{
    if (plan == NULL || step == NULL ||
        step->structure_size < sizeof(*step) ||
        step->descriptor.structure_size <
            sizeof(step->descriptor) ||
        step->descriptor.migration_id[0] == '\0' ||
        step->apply == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (plan->count >= UMI_WORKBENCH_LAYOUT_DATA_MAX_MIGRATIONS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (step->descriptor.from_version <
            plan->source_version ||
        step->descriptor.to_version >
            plan->target_version ||
        step->descriptor.from_version >=
            step->descriptor.to_version) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (step->descriptor.from_version !=
            (plan->count == 0U
                ? plan->source_version
                : plan->steps[plan->count - 1U].descriptor.to_version)) {
        return UMI_STATUS_INVALID_STATE;
    }
    plan->steps[plan->count++] = *step;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_migration_read_version(
    const UmiDataServer *server,
    uint32_t *out_version)
{
    char value[64];
    unsigned int parsed;
    UmiStatus status;
    if (server == NULL || out_version == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_version = 0U;
    status = umi_data_server_get(
        server, UMI_LAYOUT_DATA_SCHEMA_VERSION_KEY,
        value, sizeof(value));
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;
    if (sscanf(value, "%u", &parsed) != 1) {
        return UMI_STATUS_PARSE_ERROR;
    }
    *out_version = (uint32_t)parsed;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_migration_write_version(
    UmiDataServer *server,
    uint32_t version)
{
    char value[64];
    int written;
    if (server == NULL || version == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(value, sizeof(value), "%u", version);
    if (written < 0 || (size_t)written >= sizeof(value)) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    return umi_data_server_set(
        server, UMI_LAYOUT_DATA_SCHEMA_VERSION_KEY, value);
}

static UmiStatus record_state(
    UmiWorkbenchLayoutMigrationStoreRepository *records,
    const UmiWorkbenchLayoutDataMigrationStep *step,
    UmiWorkbenchLayoutDataMigrationState state,
    const char *actor_id,
    uint64_t started_at_ms,
    uint64_t completed_at_ms,
    const char *error)
{
    UmiWorkbenchLayoutMigrationRecord record;
    (void)memset(&record, 0, sizeof(record));
    record.structure_size = sizeof(record);
    record.descriptor = step->descriptor;
    record.state = state;
    record.started_at_ms = started_at_ms;
    record.completed_at_ms = completed_at_ms;
    (void)umi_workbench_layout_data_copy_text(
        record.applied_by, sizeof(record.applied_by),
        actor_id != NULL ? actor_id : "", true);
    (void)umi_workbench_layout_data_copy_text(
        record.error, sizeof(record.error),
        error != NULL ? error : "", true);
    return umi_workbench_layout_migration_store_save(
        records, &record);
}

UmiStatus umi_workbench_layout_migration_execute(
    UmiDataServer *server,
    UmiWorkbenchLayoutMigrationStoreRepository *records,
    const UmiWorkbenchLayoutDataMigrationPlan *plan,
    const char *actor_id,
    uint64_t now_ms,
    UmiWorkbenchLayoutMigrationResult *out_result)
{
    UmiWorkbenchLayoutMigrationResult result;
    uint32_t current_version = 0U;
    bool transaction_started = false;
    size_t failure_index = UMI_WORKBENCH_LAYOUT_DATA_INDEX_NONE;
    size_t index;
    UmiStatus status;
    if (server == NULL || records == NULL ||
        plan == NULL || actor_id == NULL ||
        plan->structure_size < sizeof(*plan)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&result, 0, sizeof(result));
    result.structure_size = sizeof(result);
    status = umi_workbench_layout_migration_read_version(
        server, &current_version);
    result.initial_version = current_version;
    if (status != UMI_STATUS_OK) {
        result.status = status;
        if (out_result != NULL) *out_result = result;
        return status;
    }
    if (current_version < plan->source_version ||
        current_version > plan->target_version ||
        (plan->count > 0U &&
         plan->steps[plan->count - 1U].descriptor.to_version !=
             plan->target_version)) {
        status = UMI_STATUS_INVALID_STATE;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_transaction_begin(
            server, &transaction_started);
    }
    for (index = 0U;
         status == UMI_STATUS_OK && index < plan->count;
         ++index) {
        const UmiWorkbenchLayoutDataMigrationStep *step =
            &plan->steps[index];
        if (step->descriptor.to_version <= current_version) {
            result.skipped_count += 1U;
            continue;
        }
        if (step->descriptor.from_version != current_version) {
            status = UMI_STATUS_INVALID_STATE;
            failure_index = index;
            break;
        }
        status = step->apply(server, step->context);
        if (status == UMI_STATUS_OK) {
            status = umi_workbench_layout_migration_write_version(
                server, step->descriptor.to_version);
        }
        if (status == UMI_STATUS_OK) {
            current_version = step->descriptor.to_version;
            result.applied_count += 1U;
        } else {
            failure_index = index;
        }
    }

    if (status != UMI_STATUS_OK) {
        for (index = plan->count; index > 0U; --index) {
            const UmiWorkbenchLayoutDataMigrationStep *step =
                &plan->steps[index - 1U];
            if (step->descriptor.to_version > result.initial_version &&
                step->descriptor.to_version <= current_version &&
                step->descriptor.reversible &&
                step->rollback != NULL) {
                (void)step->rollback(server, step->context);
                result.rollback_count += 1U;
            }
        }
    }
    status = umi_workbench_layout_data_transaction_finish(
        server, transaction_started, status);

    if (status == UMI_STATUS_OK) {
        for (index = 0U; index < plan->count; ++index) {
            const UmiWorkbenchLayoutDataMigrationStep *step =
                &plan->steps[index];
            if (step->descriptor.to_version <= result.initial_version) {
                continue;
            }
            status = record_state(
                records, step,
                UMI_WORKBENCH_LAYOUT_DATA_MIGRATION_APPLIED,
                actor_id, now_ms, now_ms, "");
            if (status != UMI_STATUS_OK) break;
        }
    } else {
        for (index = 0U; index < plan->count; ++index) {
            const UmiWorkbenchLayoutDataMigrationStep *step =
                &plan->steps[index];
            if (step->descriptor.to_version > result.initial_version &&
                step->descriptor.to_version <= current_version) {
                (void)record_state(
                    records, step,
                    UMI_WORKBENCH_LAYOUT_DATA_MIGRATION_ROLLED_BACK,
                    actor_id, now_ms, now_ms,
                    "Migration plan was rolled back.");
            }
        }
        current_version = result.initial_version;
        if (failure_index != UMI_WORKBENCH_LAYOUT_DATA_INDEX_NONE &&
            failure_index < plan->count) {
            const UmiWorkbenchLayoutDataMigrationStep *failed =
                &plan->steps[failure_index];
            (void)record_state(
                records, failed,
                UMI_WORKBENCH_LAYOUT_DATA_MIGRATION_FAILED,
                actor_id, now_ms, now_ms,
                "Migration operation failed.");
            (void)umi_workbench_layout_data_copy_text(
                result.failed_migration_id,
                sizeof(result.failed_migration_id),
                failed->descriptor.migration_id, true);
        }
    }

    result.status = status;
    result.resulting_version = current_version;
    (void)umi_workbench_layout_data_copy_text(
        result.message, sizeof(result.message),
        status == UMI_STATUS_OK
            ? "Layout persistence migrations completed."
            : "Layout persistence migration failed.",
        true);
    if (out_result != NULL) *out_result = result;
    return status;
}
