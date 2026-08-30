/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/store.h
 *
 * PURPOSE:
 *   Define the persistence port used by the Framework layout service. Adapters may use the Data Server, files or deterministic memory storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_STORE_H
#define UMICOM_WORKBENCH_LAYOUT_STORE_H

#include "umicom/workbench_layout/document.h"
#include "umicom/workbench_layout/session.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchLayoutRecordSummary {
    uint32_t structure_size;
    char layout_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char name[UMI_WORKBENCH_LAYOUT_NAME_CAPACITY];
    char category[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char owner_user_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char workspace_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    uint64_t revision;
    uint64_t modified_at_ms;
    uint64_t content_hash;
    uint32_t flags;
} UmiWorkbenchLayoutRecordSummary;

typedef struct UmiWorkbenchLayoutStoreQuery {
    uint32_t structure_size;
    char owner_user_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char workspace_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char category[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char text[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
    bool include_built_in;
    bool include_shared;
    bool include_read_only;
    size_t offset;
    size_t limit;
} UmiWorkbenchLayoutStoreQuery;

typedef struct UmiWorkbenchLayoutStoreList {
    uint32_t structure_size;
    UmiWorkbenchLayoutRecordSummary
        records[UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS];
    size_t count;
    size_t total_available;
    bool truncated;
} UmiWorkbenchLayoutStoreList;

typedef struct UmiWorkbenchLayoutStoreAdapter {
    uint32_t structure_size;
    UmiWorkbenchLayoutStoreKind kind;
    void *context;
    UmiStatus (*save_layout)(
        void *context,
        const UmiWorkbenchLayoutDocument *document,
        uint64_t expected_revision,
        uint64_t *out_revision);
    UmiStatus (*load_layout)(
        void *context,
        const char *layout_id,
        UmiWorkbenchLayoutDocument *out_document);
    UmiStatus (*delete_layout)(
        void *context,
        const char *layout_id,
        uint64_t expected_revision);
    UmiStatus (*list_layouts)(
        void *context,
        const UmiWorkbenchLayoutStoreQuery *query,
        UmiWorkbenchLayoutStoreList *out_list);
    UmiStatus (*save_session)(
        void *context,
        const UmiWorkbenchLayoutSession *session,
        uint64_t expected_revision,
        uint64_t *out_revision);
    UmiStatus (*load_session)(
        void *context,
        const char *session_id,
        UmiWorkbenchLayoutSession *out_session);
    UmiStatus (*delete_session)(
        void *context,
        const char *session_id,
        uint64_t expected_revision);
    UmiStatus (*begin_transaction)(void *context);
    UmiStatus (*commit_transaction)(void *context);
    UmiStatus (*rollback_transaction)(void *context);
} UmiWorkbenchLayoutStoreAdapter;

UmiWorkbenchLayoutStoreQuery
umi_workbench_layout_store_query_default(void);

UmiStatus umi_workbench_layout_store_adapter_validate(
    const UmiWorkbenchLayoutStoreAdapter *adapter);

bool umi_workbench_layout_store_supports_transactions(
    const UmiWorkbenchLayoutStoreAdapter *adapter);

UmiStatus umi_workbench_layout_store_save(
    const UmiWorkbenchLayoutStoreAdapter *adapter,
    const UmiWorkbenchLayoutDocument *document,
    uint64_t expected_revision,
    uint64_t *out_revision);

UmiStatus umi_workbench_layout_store_load(
    const UmiWorkbenchLayoutStoreAdapter *adapter,
    const char *layout_id,
    UmiWorkbenchLayoutDocument *out_document);

UmiStatus umi_workbench_layout_store_delete(
    const UmiWorkbenchLayoutStoreAdapter *adapter,
    const char *layout_id,
    uint64_t expected_revision);

UmiStatus umi_workbench_layout_store_list(
    const UmiWorkbenchLayoutStoreAdapter *adapter,
    const UmiWorkbenchLayoutStoreQuery *query,
    UmiWorkbenchLayoutStoreList *out_list);

UmiStatus umi_workbench_layout_store_save_session(
    const UmiWorkbenchLayoutStoreAdapter *adapter,
    const UmiWorkbenchLayoutSession *session,
    uint64_t expected_revision,
    uint64_t *out_revision);

UmiStatus umi_workbench_layout_store_load_session(
    const UmiWorkbenchLayoutStoreAdapter *adapter,
    const char *session_id,
    UmiWorkbenchLayoutSession *out_session);

#ifdef __cplusplus
}
#endif

#endif
