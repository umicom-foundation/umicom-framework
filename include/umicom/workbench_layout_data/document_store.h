/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/document_store.h
 *
 * PURPOSE:
 *   Persist complete semantic layout documents through the Data Server chunk store with optimistic revisions and browser summaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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


/**
 * Represent the workbench layout document store data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutDocumentStore {
    uint32_t structure_size;
    UmiDataServer *server;
    UmiWorkbenchLayoutChunkStore chunks;
} UmiWorkbenchLayoutDocumentStore;

/**
 * Initialise workbench layout document store from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_workbench_layout_document_store_init(
    UmiWorkbenchLayoutDocumentStore *store,
    UmiDataServer *server);

/**
 * Write workbench layout document store in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_workbench_layout_document_store_save(
    const UmiWorkbenchLayoutDocumentStore *store,
    const UmiWorkbenchLayoutDocument *document,
    uint64_t expected_revision,
    uint64_t modified_at_ms,
    uint64_t *out_revision);

/**
 * Read workbench layout document store into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_workbench_layout_document_store_load(
    const UmiWorkbenchLayoutDocumentStore *store,
    const char *layout_id,
    UmiWorkbenchLayoutDocument *out_document);

/**
 * Provide the workbench layout document store delete operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_store_delete(
    const UmiWorkbenchLayoutDocumentStore *store,
    const char *layout_id,
    uint64_t expected_revision);

/**
 * Provide the workbench layout document store list operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_store_list(
    const UmiWorkbenchLayoutDocumentStore *store,
    const UmiWorkbenchLayoutStoreQuery *query,
    UmiWorkbenchLayoutStoreList *out_list);

/**
 * Provide the workbench layout document store verify operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_store_verify(
    const UmiWorkbenchLayoutDocumentStore *store,
    const char *layout_id,
    UmiWorkbenchLayoutDataChunkManifest *out_manifest);

#ifdef __cplusplus
}
#endif

#endif
