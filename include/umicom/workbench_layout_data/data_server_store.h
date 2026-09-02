/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/data_server_store.h
 *
 * PURPOSE:
 *   Adapt the authoritative Umicom Data Server to the established workbench layout store port without exposing database details.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_DATA_SERVER_STORE_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_DATA_SERVER_STORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/workbench_layout/store.h"
#include "umicom/workbench_layout_data/document_store.h"
#include "umicom/workbench_layout_data/session_store.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench layout data server store data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutDataServerStore {
    uint32_t structure_size;
    UmiDataServer *server;
    UmiWorkbenchLayoutDocumentStore layouts;
    UmiWorkbenchLayoutSessionStore sessions;
    uint64_t revision;
    bool transaction_opened_by_adapter;
} UmiWorkbenchLayoutDataServerStore;

/**
 * Initialise workbench layout data server store from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_workbench_layout_data_server_store_init(
    UmiWorkbenchLayoutDataServerStore *store,
    UmiDataServer *server);

/**
 * Provide the workbench layout data server store adapter operation used by this module and
 * its client applications.
 */
UmiWorkbenchLayoutStoreAdapter
umi_workbench_layout_data_server_store_adapter(
    UmiWorkbenchLayoutDataServerStore *store);

/**
 * Provide the workbench layout data server store verify operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_server_store_verify(
    const UmiWorkbenchLayoutDataServerStore *store,
    size_t *out_layout_count,
    size_t *out_session_count);

/**
 * Provide the workbench layout data server store revision operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_layout_data_server_store_revision(
    const UmiWorkbenchLayoutDataServerStore *store);

#ifdef __cplusplus
}
#endif

#endif
