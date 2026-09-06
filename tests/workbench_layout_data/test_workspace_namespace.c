/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_workspace_namespace.c
 *
 * PURPOSE:
 *   Verify that native workspace chunks survive layout-only backup and restore,
 *   remain separate from semantic documents, and are not deleted by an
 *   incomplete reconciliation scan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/workbench_layout_data/workbench_layout_data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Keep acceptance active in release builds and release every owned fixture. */
#define REQUIRE(expression) do { \
    if (!(expression)) { \
        (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
        failed = 1; \
        goto cleanup; \
    } \
} while (0)

/* Exercise record transport rather than the product-aware UI codec. Tabs,
 * newlines and separators in chunk bytes must not be parsed as field sets. */
int main(void)
{
    static const char payload[] = "UMILAYOUT3\t3\t123\tumicom.studio.layout.test\tSaved | canvas\t1\t0\t1\n";
    UmiDataServer *source = NULL;
    UmiDataServer *destination = NULL;
    UmiWorkbenchLayoutChunkStore workspace;
    UmiWorkbenchLayoutChunkStore session;
    UmiWorkbenchLayoutDocumentStore documents;
    UmiWorkbenchLayoutStoreQuery query = umi_workbench_layout_store_query_default();
    UmiWorkbenchLayoutStoreList *list = NULL;
    UmiWorkbenchLayoutReconciliationReport *reconciliation = NULL;
    UmiWorkbenchLayoutReconciliationPolicy policy =
        umi_workbench_layout_reconciliation_policy_default();
    UmiWorkbenchLayoutBackupOptions backup_options = umi_workbench_layout_backup_options_default();
    UmiWorkbenchLayoutRestoreOptions restore_options = umi_workbench_layout_restore_options_default();
    UmiWorkbenchLayoutBackupResult sizing;
    UmiWorkbenchLayoutBackupResult backup_result;
    UmiWorkbenchLayoutRestoreResult restore_result;
    UmiWorkbenchLayoutDataChunkManifest manifest;
    UmiWorkbenchLayoutDataKeyParts parts;
    char manifest_key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char chunk_key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char orphan_key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    char *backup = NULL;
    char *restored = NULL;
    int failed = 0;

    REQUIRE(umi_data_server_create_memory(&source) == UMI_STATUS_OK);
    REQUIRE(umi_data_server_create_memory(&destination) == UMI_STATUS_OK);
    list = (UmiWorkbenchLayoutStoreList *)calloc(1U, sizeof(*list));
    reconciliation = (UmiWorkbenchLayoutReconciliationReport *)calloc(1U, sizeof(*reconciliation));
    REQUIRE(list != NULL && reconciliation != NULL);
    REQUIRE(umi_workbench_layout_chunk_store_init(&workspace, source,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK) == UMI_STATUS_OK);
    REQUIRE(umi_workbench_layout_chunk_store_save(&workspace, "workspace.native",
        payload, 1U, 123U, &manifest) == UMI_STATUS_OK);
    REQUIRE(umi_workbench_layout_chunk_store_init(&session, source,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_MANIFEST,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK) == UMI_STATUS_OK);
    REQUIRE(umi_workbench_layout_chunk_store_save(&session, "session.excluded",
        "session payload", 1U, 123U, NULL) == UMI_STATUS_OK);
    REQUIRE(umi_workbench_layout_data_key_build(UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
        "workspace.native", NULL, 0U, 0U, manifest_key, sizeof(manifest_key)) == UMI_STATUS_OK);
    REQUIRE(umi_workbench_layout_data_key_build(UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK,
        "workspace.native", NULL, 0U, 0U, chunk_key, sizeof(chunk_key)) == UMI_STATUS_OK);
    REQUIRE(umi_workbench_layout_data_key_parse(chunk_key, &parts) == UMI_STATUS_OK);
    REQUIRE(parts.kind == UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK &&
        strcmp(parts.aggregate_id, "workspace.native") == 0 && parts.chunk_index == 0U);

    /* Filtering application sessions must not exclude a saved arrangement. */
    backup_options.include_sessions = false;
    backup_options.include_outbox = false;
    REQUIRE(umi_workbench_layout_backup_create(source, "workspace-backup", "local-test", 200U,
        &backup_options, NULL, 0U, &sizing) == UMI_STATUS_CAPACITY_EXCEEDED);
    backup = (char *)calloc(sizing.bytes_required, sizeof(char));
    REQUIRE(backup != NULL);
    REQUIRE(umi_workbench_layout_backup_create(source, "workspace-backup", "local-test", 200U,
        &backup_options, backup, sizing.bytes_required, &backup_result) == UMI_STATUS_OK);
    REQUIRE(backup_result.manifest.record_count == 2U && strstr(backup, "session.excluded") == NULL);
    restore_options.restore_sessions = false;
    restore_options.validate_only = true;
    REQUIRE(umi_workbench_layout_restore_apply(destination, backup, backup_result.bytes_written,
        &restore_options, &restore_result) == UMI_STATUS_OK);
    REQUIRE(restore_result.parsed_count == 2U && restore_result.restored_count == 0U &&
        umi_data_server_count(destination) == 0U);
    restore_options.validate_only = false;
    REQUIRE(umi_workbench_layout_restore_apply(destination, backup, backup_result.bytes_written,
        &restore_options, &restore_result) == UMI_STATUS_OK);
    REQUIRE(restore_result.restored_count == 2U && restore_result.skipped_count == 0U);
    REQUIRE(umi_workbench_layout_chunk_store_init(&workspace, destination,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK) == UMI_STATUS_OK);
    REQUIRE(umi_workbench_layout_chunk_store_load(&workspace, "workspace.native", &restored,
        &manifest) == UMI_STATUS_OK);
    REQUIRE(strcmp(restored, payload) == 0 && manifest.revision == 1U);

    /* Semantic document browsing must not attempt to decode the UI format. */
    REQUIRE(umi_workbench_layout_document_store_init(&documents, destination) == UMI_STATUS_OK);
    REQUIRE(umi_workbench_layout_document_store_list(&documents, &query, list) == UMI_STATUS_OK);
    REQUIRE(list->count == 0U && list->total_available == 0U);
    REQUIRE(umi_workbench_layout_reconcile(destination, &policy, 300U, reconciliation) == UMI_STATUS_OK);
    REQUIRE(reconciliation->consistent && reconciliation->manifest_count == 1U &&
        reconciliation->chunk_count == 1U && reconciliation->issue_count == 0U);

    /* A complete inventory can repair a genuine surplus chunk safely. */
    REQUIRE(umi_workbench_layout_data_key_build(UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK,
        "workspace.native", NULL, 0U, 9U, orphan_key, sizeof(orphan_key)) == UMI_STATUS_OK);
    REQUIRE(umi_data_server_set(destination, orphan_key, "orphan") == UMI_STATUS_OK);
    policy.repair_orphan_chunks = true;
    REQUIRE(umi_workbench_layout_reconcile(destination, &policy, 400U, reconciliation) == UMI_STATUS_OK);
    REQUIRE(reconciliation->orphan_count == 1U && reconciliation->repaired_count == 1U);
    REQUIRE(umi_data_server_get(destination, orphan_key, value, sizeof(value)) == UMI_STATUS_NOT_FOUND);
    REQUIRE(umi_data_server_get(destination, chunk_key, value, sizeof(value)) == UMI_STATUS_OK);
    REQUIRE(strcmp(value, payload) == 0);

    /* A well-formed manifest stored under another identity is equally unsafe
     * for orphan repair. Its payload must not redirect chunk ownership. */
    (void)snprintf(manifest.aggregate_id, sizeof(manifest.aggregate_id), "%s", "different.workspace");
    REQUIRE(umi_workbench_layout_chunk_manifest_encode(&manifest, value, sizeof(value), NULL) == UMI_STATUS_OK);
    REQUIRE(umi_data_server_set(destination, manifest_key, value) == UMI_STATUS_OK);
    REQUIRE(umi_workbench_layout_reconcile(destination, &policy, 450U, reconciliation) == UMI_STATUS_OK);
    REQUIRE(!reconciliation->consistent && reconciliation->issue_count > 0U &&
        reconciliation->repaired_count == 0U);
    REQUIRE(umi_data_server_get(destination, chunk_key, value, sizeof(value)) == UMI_STATUS_OK);
    REQUIRE(strcmp(value, payload) == 0);

    /* Corrupt metadata is not proof that its recoverable chunks are orphaned. */
    REQUIRE(umi_data_server_set(destination, manifest_key, "invalid manifest") == UMI_STATUS_OK);
    REQUIRE(umi_workbench_layout_reconcile(destination, &policy, 500U, reconciliation) == UMI_STATUS_OK);
    REQUIRE(!reconciliation->consistent && reconciliation->issue_count > 0U &&
        reconciliation->repaired_count == 0U);
    REQUIRE(umi_data_server_get(destination, chunk_key, value, sizeof(value)) == UMI_STATUS_OK);
    REQUIRE(strcmp(value, payload) == 0);

cleanup:
    free(restored);
    free(backup);
    free(list);
    free(reconciliation);
    umi_data_server_destroy(destination);
    umi_data_server_destroy(source);
    return failed;
}
