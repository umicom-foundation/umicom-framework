/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout/test_store_persistence.c
 *
 * PURPOSE:
 *   Verify optimistic revision control, transactional session checkpoints and
 *   browser queries through the store abstraction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

int main(void)
{
    UmiWorkbenchMemoryStore *store =
        (UmiWorkbenchMemoryStore *)calloc(1U, sizeof(*store));
    UmiWorkbenchRecoveryJournal *journal =
        (UmiWorkbenchRecoveryJournal *)calloc(1U, sizeof(*journal));
    UmiWorkbenchLayoutPersistenceService *persistence = NULL;
    UmiWorkbenchLayoutDocument *document =
        test_allocate_layout("layout.persisted");
    UmiWorkbenchLayoutDocument *loaded =
        (UmiWorkbenchLayoutDocument *)calloc(1U, sizeof(*loaded));
    UmiWorkbenchLayoutDocument *failed_document =
        (UmiWorkbenchLayoutDocument *)calloc(
            1U, sizeof(*failed_document));
    UmiWorkbenchLayoutDocument *transaction_document =
        (UmiWorkbenchLayoutDocument *)calloc(
            1U, sizeof(*transaction_document));
    UmiWorkbenchLayoutDocument *rolled_back_document =
        (UmiWorkbenchLayoutDocument *)calloc(
            1U, sizeof(*rolled_back_document));
    UmiWorkbenchLayoutStoreAdapter adapter;
    UmiWorkbenchLayoutPersistenceResult result;
    UmiWorkbenchLayoutSession session;
    UmiWorkbenchLayoutBrowserQuery query;
    UmiWorkbenchLayoutBrowserResult browser;
    uint64_t saved_revision;
    uint64_t transaction_revision;

    TEST_REQUIRE(
        store != NULL && journal != NULL && document != NULL &&
        loaded != NULL && failed_document != NULL &&
        transaction_document != NULL && rolled_back_document != NULL,
        "Persistence test state must be allocated");
    umi_workbench_memory_store_init(store);
    adapter = umi_workbench_memory_store_adapter(store);
    umi_workbench_recovery_journal_init(journal);
    TEST_STATUS_OK(umi_workbench_layout_persistence_create(
        &adapter, NULL, journal, &persistence));

    TEST_STATUS_OK(umi_workbench_layout_persistence_save_layout(
        persistence,
        document,
        0U,
        "developer",
        "correlation.save",
        4000U,
        &result));
    saved_revision = result.resulting_revision;
    TEST_REQUIRE(saved_revision > 0U, "Save must return a persisted revision");
    TEST_REQUIRE(
        umi_workbench_memory_store_layout_count(store) == 1U,
        "Memory store must contain the saved layout");
    TEST_REQUIRE(
        !umi_workbench_layout_document_has_flag(
            document, UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY),
        "Successful persistence must leave the working document clean");

    TEST_STATUS_OK(umi_workbench_layout_persistence_load_layout(
        persistence, document->identity.layout_id, loaded, &result));
    TEST_REQUIRE(
        loaded->content_hash == document->content_hash,
        "Persistence round trip must preserve semantic content");
    TEST_REQUIRE(
        !umi_workbench_layout_document_has_flag(
            loaded, UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY),
        "Persisted layouts must load as clean documents");

    *failed_document = *loaded;
    TEST_STATUS_OK(umi_workbench_layout_document_set_metadata(
        failed_document,
        "Locally Modified Layout",
        failed_document->category,
        failed_document->description));
    {
        const uint64_t previous_document_revision =
            failed_document->version.revision;
        const uint64_t previous_content_hash =
            failed_document->content_hash;
        const uint64_t previous_modified_at =
            failed_document->audit.modified_at_ms;
        const uint32_t previous_flags = failed_document->flags;
        char previous_modified_by[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
        char previous_correlation_id[
            UMI_WORKBENCH_LAYOUT_ID_CAPACITY];

        test_copy_text(
            previous_modified_by,
            sizeof(previous_modified_by),
            failed_document->audit.modified_by);
        test_copy_text(
            previous_correlation_id,
            sizeof(previous_correlation_id),
            failed_document->audit.correlation_id);

        TEST_REQUIRE(
            umi_workbench_layout_persistence_save_layout(
                persistence,
                failed_document,
                saved_revision + 100U,
                "another-developer",
                "correlation.stale-save",
                4050U,
                &result) == UMI_STATUS_BUSY,
            "Optimistic save must reject a stale expected revision");
        TEST_REQUIRE(
            failed_document->version.revision ==
                previous_document_revision &&
            failed_document->content_hash == previous_content_hash &&
            failed_document->audit.modified_at_ms ==
                previous_modified_at &&
            failed_document->flags == previous_flags &&
            strcmp(
                failed_document->audit.modified_by,
                previous_modified_by) == 0 &&
            strcmp(
                failed_document->audit.correlation_id,
                previous_correlation_id) == 0,
            "Failed persistence must leave the caller document unchanged");
    }

    *transaction_document = *loaded;
    TEST_STATUS_OK(umi_workbench_layout_document_set_metadata(
        transaction_document,
        "Uncommitted Layout Name",
        transaction_document->category,
        transaction_document->description));
    TEST_STATUS_OK(adapter.begin_transaction(adapter.context));
    TEST_STATUS_OK(adapter.save_layout(
        adapter.context,
        transaction_document,
        saved_revision,
        &transaction_revision));
    TEST_REQUIRE(
        transaction_revision >= saved_revision,
        "Transactional save must expose a deterministic revision");
    TEST_STATUS_OK(adapter.rollback_transaction(adapter.context));
    TEST_STATUS_OK(adapter.load_layout(
        adapter.context,
        document->identity.layout_id,
        rolled_back_document));
    TEST_REQUIRE(
        strcmp(
            rolled_back_document->name,
            document->name) == 0,
        "Transaction rollback must restore the previous layout document");

    *transaction_document = *loaded;
    TEST_STATUS_OK(umi_workbench_layout_document_set_metadata(
        transaction_document,
        "Externally Updated Layout",
        transaction_document->category,
        transaction_document->description));
    TEST_STATUS_OK(adapter.save_layout(
        adapter.context,
        transaction_document,
        saved_revision,
        &transaction_revision));

    umi_workbench_layout_session_init(
        &session,
        "session.persisted",
        "developer",
        "workspace.framework");
    TEST_STATUS_OK(umi_workbench_layout_session_set_active_layout(
        &session, document->identity.layout_id));
    {
        const uint64_t previous_document_revision =
            document->version.revision;
        const uint64_t previous_modified_at =
            document->audit.modified_at_ms;
        const uint64_t previous_session_revision = session.revision;
        const uint64_t previous_checkpoint_at =
            session.last_checkpoint_at_ms;
        const uint32_t previous_flags = document->flags;

        TEST_REQUIRE(
            umi_workbench_layout_persistence_checkpoint_session(
                persistence,
                document,
                &session,
                "developer",
                "correlation.stale-checkpoint",
                4075U,
                &result) == UMI_STATUS_BUSY,
            "Session checkpoint must reject a stale layout revision");
        TEST_REQUIRE(
            document->version.revision ==
                previous_document_revision &&
            document->audit.modified_at_ms == previous_modified_at &&
            document->flags == previous_flags &&
            session.revision == previous_session_revision &&
            session.last_checkpoint_at_ms == previous_checkpoint_at,
            "Failed checkpoint must leave caller layout and session unchanged");
        TEST_REQUIRE(
            umi_workbench_memory_store_session_count(store) == 0U,
            "Rolled-back checkpoint must not persist a session");
    }

    TEST_STATUS_OK(umi_workbench_layout_persistence_load_layout(
        persistence, document->identity.layout_id, document, &result));

    TEST_REQUIRE(
        umi_workbench_layout_persistence_delete_layout(
            persistence,
            document->identity.layout_id,
            saved_revision + 10U,
            &result) == UMI_STATUS_BUSY,
        "Optimistic delete must reject a stale expected revision");

    TEST_STATUS_OK(umi_workbench_layout_persistence_checkpoint_session(
        persistence,
        document,
        &session,
        "developer",
        "correlation.checkpoint",
        4100U,
        &result));
    TEST_REQUIRE(result.transaction_used, "Checkpoint must use store transaction");
    TEST_REQUIRE(
        umi_workbench_memory_store_session_count(store) == 1U,
        "Memory store must persist the active session");

    query = umi_workbench_layout_browser_query_default();
    test_copy_text(query.text, sizeof(query.text), "development");
    TEST_STATUS_OK(umi_workbench_layout_browser_search(
        &adapter,
        &query,
        "developer",
        document->identity.layout_id,
        &browser));
    TEST_REQUIRE(browser.count == 1U, "Browser must find persisted layout");
    TEST_REQUIRE(browser.items[0].active, "Browser must mark active layout");

    umi_workbench_layout_persistence_destroy(persistence);
    free(rolled_back_document);
    free(transaction_document);
    free(failed_document);
    free(loaded);
    free(document);
    free(journal);
    umi_workbench_memory_store_clear(store);
    free(store);
    return 0;
}
