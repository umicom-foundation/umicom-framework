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

/**
 * Represent the workbench memory layout record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchMemoryLayoutRecord {
    UmiWorkbenchLayoutDocument *document;
    bool occupied;
} UmiWorkbenchMemoryLayoutRecord;

/**
 * Represent the workbench memory session record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchMemorySessionRecord {
    UmiWorkbenchLayoutSession *session;
    bool occupied;
} UmiWorkbenchMemorySessionRecord;

/**
 * Represent the workbench memory store data shared with callers of this public contract.
 */
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

/**
 * Initialise workbench memory store from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_memory_store_init(
    UmiWorkbenchMemoryStore *store);

/**
 * Provide the workbench memory store adapter operation used by this module and its client
 * applications.
 */
UmiWorkbenchLayoutStoreAdapter umi_workbench_memory_store_adapter(
    UmiWorkbenchMemoryStore *store);

/**
 * Return the number of records represented by workbench memory store layout without
 * changing their state.
 */
size_t umi_workbench_memory_store_layout_count(
    const UmiWorkbenchMemoryStore *store);

/**
 * Return the number of records represented by workbench memory store session without
 * changing their state.
 */
size_t umi_workbench_memory_store_session_count(
    const UmiWorkbenchMemoryStore *store);

/**
 * Release or reset state held by workbench memory store so the same storage can be reused
 * safely.
 */
void umi_workbench_memory_store_clear(
    UmiWorkbenchMemoryStore *store);

#ifdef __cplusplus
}
#endif

#endif
