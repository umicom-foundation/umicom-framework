/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/task.h
 *
 * PURPOSE:
 *   Define a reusable project-system record used by Studio and future Umicom development products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_PROJECT_TASK_H
#define UMICOM_PROJECT_TASK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_TASK_CAPACITY 1024U
#define UMI_PROJECT_TASK_API_VERSION 1U

/**
 * Represent the project task snapshot data shared with callers of this public contract.
 */
typedef struct UmiProjectTaskSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char label[256];
    char command[1024];
    char working_directory[1024];
    char group[128];
    int default_task;
    int background;
    int enabled;
    uint64_t revision;
} UmiProjectTaskSnapshot;

/**
 * Represent the project task registry data shared with callers of this public contract.
 */
typedef struct UmiProjectTaskRegistry UmiProjectTaskRegistry;

/**
 * Initialise project task registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_project_task_registry_create(UmiProjectTaskRegistry **out_registry);
/**
 * Release or reset state held by project task registry so the same storage can be reused
 * safely.
 */
void umi_project_task_registry_destroy(UmiProjectTaskRegistry *registry);
/**
 * Provide the project task registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_project_task_registry_upsert(UmiProjectTaskRegistry *registry, const UmiProjectTaskSnapshot *item);
/**
 * Remove project task registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_task_registry_remove(UmiProjectTaskRegistry *registry, const char *id);
/**
 * Find project task registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_project_task_registry_find(const UmiProjectTaskRegistry *registry, const char *id, UmiProjectTaskSnapshot *out_item);
/**
 * Find project task registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_project_task_registry_at(const UmiProjectTaskRegistry *registry, size_t index, UmiProjectTaskSnapshot *out_item);
/**
 * Return the number of records represented by project task registry without changing their
 * state.
 */
size_t umi_project_task_registry_count(const UmiProjectTaskRegistry *registry);
/**
 * Provide the project task registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_project_task_registry_revision(const UmiProjectTaskRegistry *registry);
/**
 * Release or reset state held by project task registry so the same storage can be reused
 * safely.
 */
void umi_project_task_registry_clear(UmiProjectTaskRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_project_task_snapshot_validate(const UmiProjectTaskSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_PROJECT_TASK_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_project_task_registry_upsert_many(UmiProjectTaskRegistry *registry,
    const UmiProjectTaskSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
