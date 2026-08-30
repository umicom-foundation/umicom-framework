/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/memory_store.h
 *
 * PURPOSE:
 *   Provide a deterministic in-memory layout store for tests, previews, headless tools and products that have not configured a Data Server.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_MEMORY_STORE_H
#define UMICOM_WORKBENCH_LAYOUT_MEMORY_STORE_H

#include "umicom/workbench_layout/store.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WORKBENCH_MEMORY_STORE_MAX_SESSIONS 64U

typedef struct UmiWorkbenchMemoryLayoutRecord {
    UmiWorkbenchLayoutDocument *document;
    bool occupied;
} UmiWorkbenchMemoryLayoutRecord;

typedef struct UmiWorkbenchMemorySessionRecord {
    UmiWorkbenchLayoutSession *session;
    bool occupied;
} UmiWorkbenchMemorySessionRecord;

typedef struct UmiWorkbenchMemoryStore {
    uint32_t structure_size;
    UmiWorkbenchMemoryLayoutRecord
        layouts[UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS];
    UmiWorkbenchMemorySessionRecord
        sessions[UMI_WORKBENCH_MEMORY_STORE_MAX_SESSIONS];
    UmiWorkbenchMemoryLayoutRecord
        transaction_layouts[UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS];
    UmiWorkbenchMemorySessionRecord
        transaction_sessions[UMI_WORKBENCH_MEMORY_STORE_MAX_SESSIONS];
    uint64_t revision;
    bool transaction_open;
} UmiWorkbenchMemoryStore;

void umi_workbench_memory_store_init(
    UmiWorkbenchMemoryStore *store);

UmiWorkbenchLayoutStoreAdapter umi_workbench_memory_store_adapter(
    UmiWorkbenchMemoryStore *store);

size_t umi_workbench_memory_store_layout_count(
    const UmiWorkbenchMemoryStore *store);

size_t umi_workbench_memory_store_session_count(
    const UmiWorkbenchMemoryStore *store);

void umi_workbench_memory_store_clear(
    UmiWorkbenchMemoryStore *store);

#ifdef __cplusplus
}
#endif

#endif
