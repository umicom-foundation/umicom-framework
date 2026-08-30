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


typedef struct UmiWorkbenchLayoutDataServerStore {
    uint32_t structure_size;
    UmiDataServer *server;
    UmiWorkbenchLayoutDocumentStore layouts;
    UmiWorkbenchLayoutSessionStore sessions;
    uint64_t revision;
    bool transaction_opened_by_adapter;
} UmiWorkbenchLayoutDataServerStore;

UmiStatus umi_workbench_layout_data_server_store_init(
    UmiWorkbenchLayoutDataServerStore *store,
    UmiDataServer *server);

UmiWorkbenchLayoutStoreAdapter
umi_workbench_layout_data_server_store_adapter(
    UmiWorkbenchLayoutDataServerStore *store);

UmiStatus umi_workbench_layout_data_server_store_verify(
    const UmiWorkbenchLayoutDataServerStore *store,
    size_t *out_layout_count,
    size_t *out_session_count);

uint64_t umi_workbench_layout_data_server_store_revision(
    const UmiWorkbenchLayoutDataServerStore *store);

#ifdef __cplusplus
}
#endif

#endif
