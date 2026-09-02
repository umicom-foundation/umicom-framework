/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/chunk_store.h
 *
 * PURPOSE:
 *   Store large layout and session payloads as bounded Data Server chunks with transactional manifests and integrity evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_CHUNK_STORE_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_CHUNK_STORE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench layout chunk store data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutChunkStore {
    uint32_t structure_size;
    UmiDataServer *server;
    UmiWorkbenchLayoutDataRecordKind manifest_kind;
    UmiWorkbenchLayoutDataRecordKind chunk_kind;
} UmiWorkbenchLayoutChunkStore;

/**
 * Initialise workbench layout chunk store from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_workbench_layout_chunk_store_init(
    UmiWorkbenchLayoutChunkStore *store,
    UmiDataServer *server,
    UmiWorkbenchLayoutDataRecordKind manifest_kind,
    UmiWorkbenchLayoutDataRecordKind chunk_kind);

/**
 * Write workbench layout chunk store in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_layout_chunk_store_save(
    const UmiWorkbenchLayoutChunkStore *store,
    const char *aggregate_id,
    const char *text,
    uint64_t revision,
    uint64_t modified_at_ms,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest);

/**
 * Read workbench layout chunk store into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_chunk_store_load(
    const UmiWorkbenchLayoutChunkStore *store,
    const char *aggregate_id,
    char **out_text,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest);

/**
 * Provide the workbench layout chunk store delete operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_chunk_store_delete(
    const UmiWorkbenchLayoutChunkStore *store,
    const char *aggregate_id);

/**
 * Provide the workbench layout chunk store verify operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_chunk_store_verify(
    const UmiWorkbenchLayoutChunkStore *store,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest);

/**
 * Write workbench layout chunk manifest in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_workbench_layout_chunk_manifest_encode(
    const UmiWorkbenchLayoutDataChunkManifest *manifest,
    char *buffer,
    size_t capacity,
    size_t *out_required);

/**
 * Read workbench layout chunk manifest into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_workbench_layout_chunk_manifest_decode(
    const char *value,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest);

#ifdef __cplusplus
}
#endif

#endif
