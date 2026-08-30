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


typedef struct UmiWorkbenchLayoutChunkStore {
    uint32_t structure_size;
    UmiDataServer *server;
    UmiWorkbenchLayoutDataRecordKind manifest_kind;
    UmiWorkbenchLayoutDataRecordKind chunk_kind;
} UmiWorkbenchLayoutChunkStore;

UmiStatus umi_workbench_layout_chunk_store_init(
    UmiWorkbenchLayoutChunkStore *store,
    UmiDataServer *server,
    UmiWorkbenchLayoutDataRecordKind manifest_kind,
    UmiWorkbenchLayoutDataRecordKind chunk_kind);

UmiStatus umi_workbench_layout_chunk_store_save(
    const UmiWorkbenchLayoutChunkStore *store,
    const char *aggregate_id,
    const char *text,
    uint64_t revision,
    uint64_t modified_at_ms,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest);

UmiStatus umi_workbench_layout_chunk_store_load(
    const UmiWorkbenchLayoutChunkStore *store,
    const char *aggregate_id,
    char **out_text,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest);

UmiStatus umi_workbench_layout_chunk_store_delete(
    const UmiWorkbenchLayoutChunkStore *store,
    const char *aggregate_id);

UmiStatus umi_workbench_layout_chunk_store_verify(
    const UmiWorkbenchLayoutChunkStore *store,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest);

UmiStatus umi_workbench_layout_chunk_manifest_encode(
    const UmiWorkbenchLayoutDataChunkManifest *manifest,
    char *buffer,
    size_t capacity,
    size_t *out_required);

UmiStatus umi_workbench_layout_chunk_manifest_decode(
    const char *value,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest);

#ifdef __cplusplus
}
#endif

#endif
