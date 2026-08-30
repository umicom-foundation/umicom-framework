/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/data_server_store.c
 *
 * PURPOSE:
 *   Implement the established layout store adapter over the Umicom Data Server with document, session and transaction delegation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/data_server_store.h"
#include "umicom/workbench_layout_data/key_codec.h"
#include <string.h>
#include "internal.h"




static UmiStatus adapter_save_layout(
    void *context,
    const UmiWorkbenchLayoutDocument *document,
    uint64_t expected_revision,
    uint64_t *out_revision)
{
    UmiWorkbenchLayoutDataServerStore *store =
        (UmiWorkbenchLayoutDataServerStore *)context;
    UmiStatus status;
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_layout_document_store_save(
        &store->layouts, document, expected_revision,
        document != NULL ? document->audit.modified_at_ms : 0U,
        out_revision);
    if (status == UMI_STATUS_OK) store->revision += 1U;
    return status;
}

static UmiStatus adapter_load_layout(
    void *context,
    const char *layout_id,
    UmiWorkbenchLayoutDocument *out_document)
{
    UmiWorkbenchLayoutDataServerStore *store =
        (UmiWorkbenchLayoutDataServerStore *)context;
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_workbench_layout_document_store_load(
        &store->layouts, layout_id, out_document);
}

static UmiStatus adapter_delete_layout(
    void *context,
    const char *layout_id,
    uint64_t expected_revision)
{
    UmiWorkbenchLayoutDataServerStore *store =
        (UmiWorkbenchLayoutDataServerStore *)context;
    UmiStatus status;
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_layout_document_store_delete(
        &store->layouts, layout_id, expected_revision);
    if (status == UMI_STATUS_OK) store->revision += 1U;
    return status;
}

static UmiStatus adapter_list_layouts(
    void *context,
    const UmiWorkbenchLayoutStoreQuery *query,
    UmiWorkbenchLayoutStoreList *out_list)
{
    UmiWorkbenchLayoutDataServerStore *store =
        (UmiWorkbenchLayoutDataServerStore *)context;
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_workbench_layout_document_store_list(
        &store->layouts, query, out_list);
}

static UmiStatus adapter_save_session(
    void *context,
    const UmiWorkbenchLayoutSession *session,
    uint64_t expected_revision,
    uint64_t *out_revision)
{
    UmiWorkbenchLayoutDataServerStore *store =
        (UmiWorkbenchLayoutDataServerStore *)context;
    UmiStatus status;
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_layout_session_store_save(
        &store->sessions, session, expected_revision, out_revision);
    if (status == UMI_STATUS_OK) store->revision += 1U;
    return status;
}

static UmiStatus adapter_load_session(
    void *context,
    const char *session_id,
    UmiWorkbenchLayoutSession *out_session)
{
    UmiWorkbenchLayoutDataServerStore *store =
        (UmiWorkbenchLayoutDataServerStore *)context;
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_workbench_layout_session_store_load(
        &store->sessions, session_id, out_session);
}

static UmiStatus adapter_delete_session(
    void *context,
    const char *session_id,
    uint64_t expected_revision)
{
    UmiWorkbenchLayoutDataServerStore *store =
        (UmiWorkbenchLayoutDataServerStore *)context;
    UmiStatus status;
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_layout_session_store_delete(
        &store->sessions, session_id, expected_revision);
    if (status == UMI_STATUS_OK) store->revision += 1U;
    return status;
}

static UmiStatus adapter_begin(void *context)
{
    UmiWorkbenchLayoutDataServerStore *store =
        (UmiWorkbenchLayoutDataServerStore *)context;
    UmiStatus status;
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_data_server_in_transaction(store->server)) {
        store->transaction_opened_by_adapter = false;
        return UMI_STATUS_OK;
    }
    status = umi_data_server_begin(store->server);
    if (status == UMI_STATUS_OK) {
        store->transaction_opened_by_adapter = true;
    }
    return status;
}

static UmiStatus adapter_commit(void *context)
{
    UmiWorkbenchLayoutDataServerStore *store =
        (UmiWorkbenchLayoutDataServerStore *)context;
    UmiStatus status;
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!store->transaction_opened_by_adapter) return UMI_STATUS_OK;
    status = umi_data_server_commit(store->server);
    if (status == UMI_STATUS_OK) {
        store->transaction_opened_by_adapter = false;
        store->revision += 1U;
    }
    return status;
}

static UmiStatus adapter_rollback(void *context)
{
    UmiWorkbenchLayoutDataServerStore *store =
        (UmiWorkbenchLayoutDataServerStore *)context;
    UmiStatus status;
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!store->transaction_opened_by_adapter) return UMI_STATUS_OK;
    status = umi_data_server_rollback(store->server);
    if (status == UMI_STATUS_OK) {
        store->transaction_opened_by_adapter = false;
    }
    return status;
}

UmiStatus umi_workbench_layout_data_server_store_init(
    UmiWorkbenchLayoutDataServerStore *store,
    UmiDataServer *server)
{
    UmiStatus status;
    if (store == NULL || server == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(store, 0, sizeof(*store));
    store->structure_size = sizeof(*store);
    store->server = server;
    store->revision = 1U;
    status = umi_workbench_layout_document_store_init(
        &store->layouts, server);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_session_store_init(
            &store->sessions, server);
    }
    return status;
}

UmiWorkbenchLayoutStoreAdapter
umi_workbench_layout_data_server_store_adapter(
    UmiWorkbenchLayoutDataServerStore *store)
{
    UmiWorkbenchLayoutStoreAdapter adapter;
    (void)memset(&adapter, 0, sizeof(adapter));
    adapter.structure_size = sizeof(adapter);
    adapter.kind = UMI_WORKBENCH_LAYOUT_STORE_DATA_SERVER;
    adapter.context = store;
    adapter.save_layout = adapter_save_layout;
    adapter.load_layout = adapter_load_layout;
    adapter.delete_layout = adapter_delete_layout;
    adapter.list_layouts = adapter_list_layouts;
    adapter.save_session = adapter_save_session;
    adapter.load_session = adapter_load_session;
    adapter.delete_session = adapter_delete_session;
    adapter.begin_transaction = adapter_begin;
    adapter.commit_transaction = adapter_commit;
    adapter.rollback_transaction = adapter_rollback;
    return adapter;
}

typedef struct VerifyContext {
    size_t layout_count;
    size_t session_count;
    UmiStatus status;
} VerifyContext;

static UmiStatus verify_accept(
    const char *key,
    const char *value,
    void *context)
{
    VerifyContext *verify = (VerifyContext *)context;
    UmiWorkbenchLayoutDataKeyParts parts;
    UmiWorkbenchLayoutDataChunkManifest manifest;
    UmiStatus status;
    if (verify == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_layout_data_key_parse(key, &parts);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_chunk_manifest_decode(
        value, &manifest);
    if (status != UMI_STATUS_OK) return status;
    if (parts.kind ==
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST) {
        verify->layout_count += 1U;
    } else if (parts.kind ==
               UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_MANIFEST) {
        verify->session_count += 1U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_data_server_store_verify(
    const UmiWorkbenchLayoutDataServerStore *store,
    size_t *out_layout_count,
    size_t *out_session_count)
{
    char prefix[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    VerifyContext verify;
    UmiStatus status;
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(&verify, 0, sizeof(verify));
    status = umi_workbench_layout_data_key_prefix(
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST,
        NULL, prefix, sizeof(prefix));
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_store_visit_prefix(
            store->server, prefix, verify_accept, &verify, NULL);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_key_prefix(
            UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_MANIFEST,
            NULL, prefix, sizeof(prefix));
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_store_visit_prefix(
            store->server, prefix, verify_accept, &verify, NULL);
    }
    if (out_layout_count != NULL) {
        *out_layout_count = verify.layout_count;
    }
    if (out_session_count != NULL) {
        *out_session_count = verify.session_count;
    }
    return status;
}

uint64_t umi_workbench_layout_data_server_store_revision(
    const UmiWorkbenchLayoutDataServerStore *store)
{
    return store != NULL ? store->revision : 0U;
}
