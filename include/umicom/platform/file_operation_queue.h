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

/* BEGINNER NOTE:
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_PLATFORM_FILE_OPERATION_QUEUE_H
#define UMICOM_PLATFORM_FILE_OPERATION_QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLATFORM_FILE_OPERATION_QUEUE_CAPACITY 512U

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

typedef struct UmiFileOperationRegistry UmiFileOperationRegistry;

UmiStatus umi_platform_file_operation_queue_registry_create(UmiFileOperationRegistry **out_registry);
void umi_platform_file_operation_queue_registry_destroy(UmiFileOperationRegistry *registry);
UmiStatus umi_platform_file_operation_queue_registry_upsert(UmiFileOperationRegistry *registry, const UmiFileOperationSnapshot *item);
UmiStatus umi_platform_file_operation_queue_registry_remove(UmiFileOperationRegistry *registry, const char *id);
UmiStatus umi_platform_file_operation_queue_registry_find(const UmiFileOperationRegistry *registry, const char *id, UmiFileOperationSnapshot *out_item);
UmiStatus umi_platform_file_operation_queue_registry_at(const UmiFileOperationRegistry *registry, size_t index, UmiFileOperationSnapshot *out_item);
UmiStatus umi_platform_file_operation_queue_registry_update_progress(
    UmiFileOperationRegistry *registry,
    const char *id,
    uint64_t bytes_done,
    int state,
    const char *error_text);
size_t umi_platform_file_operation_queue_registry_count(const UmiFileOperationRegistry *registry);
uint64_t umi_platform_file_operation_queue_registry_revision(const UmiFileOperationRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
