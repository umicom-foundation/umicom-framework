/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/file_operation_queue.h
 *
 * PURPOSE:
 *   Define deterministic queued file operations suitable for file-manager and IDE workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_PLATFORM_FILE_OPERATION_QUEUE_H
#define UMICOM_PLATFORM_FILE_OPERATION_QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLATFORM_FILE_OPERATION_QUEUE_CAPACITY 512U

/**
 * Represent the file operation snapshot data shared with callers of this public contract.
 */
typedef struct UmiFileOperationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char operation[64];
    char source_uri[1024];
    char target_uri[1024];
    char error_text[512];
    uint64_t bytes_total;
    uint64_t bytes_done;
    int state;
    int cancellable;
    int overwrite;
    uint64_t revision;
} UmiFileOperationSnapshot;

/**
 * Represent the file operation registry data shared with callers of this public contract.
 */
typedef struct UmiFileOperationRegistry UmiFileOperationRegistry;

/**
 * Initialise platform file operation queue registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_platform_file_operation_queue_registry_create(UmiFileOperationRegistry **out_registry);
/**
 * Release or reset state held by platform file operation queue registry so the same
 * storage can be reused safely.
 */
void umi_platform_file_operation_queue_registry_destroy(UmiFileOperationRegistry *registry);
/**
 * Provide the platform file operation queue registry upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_platform_file_operation_queue_registry_upsert(UmiFileOperationRegistry *registry, const UmiFileOperationSnapshot *item);
/**
 * Remove platform file operation queue registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_platform_file_operation_queue_registry_remove(UmiFileOperationRegistry *registry, const char *id);
/**
 * Find platform file operation queue registry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_platform_file_operation_queue_registry_find(const UmiFileOperationRegistry *registry, const char *id, UmiFileOperationSnapshot *out_item);
/**
 * Find platform file operation queue registry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_platform_file_operation_queue_registry_at(const UmiFileOperationRegistry *registry, size_t index, UmiFileOperationSnapshot *out_item);
/**
 * Provide the platform file operation queue registry update progress operation used by
 * this module and its client applications.
 */
UmiStatus umi_platform_file_operation_queue_registry_update_progress(
    UmiFileOperationRegistry *registry,
    const char *id,
    uint64_t bytes_done,
    int state,
    const char *error_text);
/**
 * Return the number of records represented by platform file operation queue registry
 * without changing their state.
 */
size_t umi_platform_file_operation_queue_registry_count(const UmiFileOperationRegistry *registry);
/**
 * Provide the platform file operation queue registry revision operation used by this
 * module and its client applications.
 */
uint64_t umi_platform_file_operation_queue_registry_revision(const UmiFileOperationRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_platform_file_operation_queue_snapshot_validate(const UmiFileOperationSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_PLATFORM_FILE_OPERATION_QUEUE_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_platform_file_operation_queue_registry_upsert_many(UmiFileOperationRegistry *registry,
    const UmiFileOperationSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
