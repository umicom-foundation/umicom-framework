/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/store.c
 *
 * PURPOSE:
 *   Implement validated persistence-port dispatch for Data Server, file and deterministic in-memory layout stores.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/store.h"

#include <string.h>

#include "internal.h"

/*
 * Provide the workbench layout store query default operation used by this module and its
 * client applications.
 */
UmiWorkbenchLayoutStoreQuery
umi_workbench_layout_store_query_default(void)
{
    UmiWorkbenchLayoutStoreQuery query;

    (void)memset(&query, 0, sizeof(query));
    query.structure_size = sizeof(query);
    query.include_built_in = true;
    query.include_shared = true;
    query.include_read_only = true;
    query.offset = 0U;
    query.limit = UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS;
    return query;
}

/*
 * Check that workbench layout store adapter satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_layout_store_adapter_validate(
    const UmiWorkbenchLayoutStoreAdapter *adapter)
{
    bool any_transaction;
    bool all_transaction;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL ||
        adapter->structure_size < sizeof(*adapter) ||
        adapter->kind < UMI_WORKBENCH_LAYOUT_STORE_MEMORY ||
        adapter->kind > UMI_WORKBENCH_LAYOUT_STORE_FILE ||
        adapter->context == NULL ||
        adapter->save_layout == NULL ||
        adapter->load_layout == NULL ||
        adapter->delete_layout == NULL ||
        adapter->list_layouts == NULL ||
        adapter->save_session == NULL ||
        adapter->load_session == NULL ||
        adapter->delete_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    any_transaction =
        adapter->begin_transaction != NULL ||
        adapter->commit_transaction != NULL ||
        adapter->rollback_transaction != NULL;
    all_transaction =
        adapter->begin_transaction != NULL &&
        adapter->commit_transaction != NULL &&
        adapter->rollback_transaction != NULL;

    return !any_transaction || all_transaction
        ? UMI_STATUS_OK
        : UMI_STATUS_INVALID_ARGUMENT;
}

/*
 * Provide the workbench layout store supports transactions operation used by this module
 * and its client applications.
 */
bool umi_workbench_layout_store_supports_transactions(
    const UmiWorkbenchLayoutStoreAdapter *adapter)
{
    return adapter != NULL &&
           adapter->begin_transaction != NULL &&
           adapter->commit_transaction != NULL &&
           adapter->rollback_transaction != NULL;
}

/*
 * Write workbench layout store in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_workbench_layout_store_save(
    const UmiWorkbenchLayoutStoreAdapter *adapter,
    const UmiWorkbenchLayoutDocument *document,
    uint64_t expected_revision,
    uint64_t *out_revision)
{
    UmiStatus status =
        umi_workbench_layout_store_adapter_validate(adapter);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK || document == NULL ||
        out_revision == NULL) {
        return status != UMI_STATUS_OK
            ? status
            : UMI_STATUS_INVALID_ARGUMENT;
    }
    return adapter->save_layout(
        adapter->context,
        document,
        expected_revision,
        out_revision);
}

/*
 * Read workbench layout store into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_workbench_layout_store_load(
    const UmiWorkbenchLayoutStoreAdapter *adapter,
    const char *layout_id,
    UmiWorkbenchLayoutDocument *out_document)
{
    UmiStatus status =
        umi_workbench_layout_store_adapter_validate(adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK ||
        !umi_workbench_layout_text_present(layout_id) ||
        out_document == NULL) {
        return status != UMI_STATUS_OK
            ? status
            : UMI_STATUS_INVALID_ARGUMENT;
    }
    return adapter->load_layout(
        adapter->context, layout_id, out_document);
}

/*
 * Provide the workbench layout store delete operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_store_delete(
    const UmiWorkbenchLayoutStoreAdapter *adapter,
    const char *layout_id,
    uint64_t expected_revision)
{
    UmiStatus status =
        umi_workbench_layout_store_adapter_validate(adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK ||
        !umi_workbench_layout_text_present(layout_id)) {
        return status != UMI_STATUS_OK
            ? status
            : UMI_STATUS_INVALID_ARGUMENT;
    }
    return adapter->delete_layout(
        adapter->context, layout_id, expected_revision);
}

/*
 * Provide the workbench layout store list operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_store_list(
    const UmiWorkbenchLayoutStoreAdapter *adapter,
    const UmiWorkbenchLayoutStoreQuery *query,
    UmiWorkbenchLayoutStoreList *out_list)
{
    UmiWorkbenchLayoutStoreQuery effective;
    UmiStatus status =
        umi_workbench_layout_store_adapter_validate(adapter);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK || out_list == NULL) {
        return status != UMI_STATUS_OK
            ? status
            : UMI_STATUS_INVALID_ARGUMENT;
    }
    effective = query != NULL
        ? *query
        : umi_workbench_layout_store_query_default();
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (effective.limit == 0U ||
        effective.limit >
            UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS) {
        effective.limit =
            UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS;
    }
    return adapter->list_layouts(
        adapter->context, &effective, out_list);
}

/*
 * Provide the workbench layout store save session operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_store_save_session(
    const UmiWorkbenchLayoutStoreAdapter *adapter,
    const UmiWorkbenchLayoutSession *session,
    uint64_t expected_revision,
    uint64_t *out_revision)
{
    UmiStatus status =
        umi_workbench_layout_store_adapter_validate(adapter);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK || session == NULL ||
        out_revision == NULL) {
        return status != UMI_STATUS_OK
            ? status
            : UMI_STATUS_INVALID_ARGUMENT;
    }
    return adapter->save_session(
        adapter->context,
        session,
        expected_revision,
        out_revision);
}

/*
 * Provide the workbench layout store load session operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_store_load_session(
    const UmiWorkbenchLayoutStoreAdapter *adapter,
    const char *session_id,
    UmiWorkbenchLayoutSession *out_session)
{
    UmiStatus status =
        umi_workbench_layout_store_adapter_validate(adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK ||
        !umi_workbench_layout_text_present(session_id) ||
        out_session == NULL) {
        return status != UMI_STATUS_OK
            ? status
            : UMI_STATUS_INVALID_ARGUMENT;
    }
    return adapter->load_session(
        adapter->context, session_id, out_session);
}
