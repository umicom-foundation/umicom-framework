/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/document_store.h
 *
 * PURPOSE:
 *   Persist complete semantic layout documents through the Data Server chunk store with optimistic revisions and browser summaries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_DOCUMENT_STORE_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_DOCUMENT_STORE_H

#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout/store.h"
#include "umicom/workbench_layout_data/chunk_store.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchLayoutDocumentStore {
    uint32_t structure_size;
    UmiDataServer *server;
    UmiWorkbenchLayoutChunkStore chunks;
} UmiWorkbenchLayoutDocumentStore;

UmiStatus umi_workbench_layout_document_store_init(
    UmiWorkbenchLayoutDocumentStore *store,
    UmiDataServer *server);

UmiStatus umi_workbench_layout_document_store_save(
    const UmiWorkbenchLayoutDocumentStore *store,
    const UmiWorkbenchLayoutDocument *document,
    uint64_t expected_revision,
    uint64_t modified_at_ms,
    uint64_t *out_revision);

UmiStatus umi_workbench_layout_document_store_load(
    const UmiWorkbenchLayoutDocumentStore *store,
    const char *layout_id,
    UmiWorkbenchLayoutDocument *out_document);

UmiStatus umi_workbench_layout_document_store_delete(
    const UmiWorkbenchLayoutDocumentStore *store,
    const char *layout_id,
    uint64_t expected_revision);

UmiStatus umi_workbench_layout_document_store_list(
    const UmiWorkbenchLayoutDocumentStore *store,
    const UmiWorkbenchLayoutStoreQuery *query,
    UmiWorkbenchLayoutStoreList *out_list);

UmiStatus umi_workbench_layout_document_store_verify(
    const UmiWorkbenchLayoutDocumentStore *store,
    const char *layout_id,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest);

#ifdef __cplusplus
}
#endif

#endif
