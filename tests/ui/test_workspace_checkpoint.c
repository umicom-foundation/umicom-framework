/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui/test_workspace_checkpoint.c
 * PURPOSE: Verify native canvas durability, explicit revision conflicts and
 *          failure-safe recovery using the existing Data Server and UI codec.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/test_runtime/check.h"
#include "umicom/ui/workspace_checkpoint.h"
#include "umicom/workbench_layout_data/chunk_store.h"
#include "umicom/workbench_layout_data/key_codec.h"

static const UmiUiWorkspaceCheckpointScope scope = {
    "org.umicom.fixture", "developer", "org.umicom.fixture."};
static const char primary_id[] = "org.umicom.fixture@developer@primary";
static const char backup_id[] = "org.umicom.fixture@developer@last-good";

/* Build one registered native tool and a committed free-placement layout.
 * Large layout collections stay on the heap to respect Windows stack limits. */
static int seed_model(UmiUiWorkspaceCustomisation *model)
{
    UmiUiWindowDescriptor tool = {0};
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    umi_ui_workspace_customisation_init(model);
    memcpy(tool.tool_id, "editor", sizeof("editor"));
    memcpy(tool.title, "Editor", sizeof("Editor"));
    tool.category = UMI_UI_WINDOW_CATEGORY_DEVELOPMENT;
    tool.default_width = 0.40;
    tool.default_height = 0.50;
    UMI_TEST_REQUIRE(umi_ui_window_catalogue_register(&model->windows, &tool) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_create_blank_layout(model,
        "org.umicom.fixture.custom", "Saved canvas") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_open_window(model, "editor",
        "canvas", false, 10U, window_id, sizeof(window_id)) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_place_canvas_window(model,
        "editor", 0.125, 0.25, 0.50, 0.50) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_commit_edit(model) == UMI_STATUS_OK);
    return EXIT_SUCCESS;
}

/* Use the public namespace codec even when deliberately damaging a fixture;
 * tests must not depend on a private handwritten database-key parser. */
static int record_key(UmiWorkbenchLayoutDataRecordKind kind, const char *aggregate,
                      uint64_t revision, char *out_key, size_t capacity)
{
    UMI_TEST_REQUIRE(umi_workbench_layout_data_key_build(kind, aggregate, NULL,
        revision, 0U, out_key, capacity) == UMI_STATUS_OK);
    return EXIT_SUCCESS;
}

/* Check staged load, compare-and-swap, recovery and isolation against one
 * memory server. Loading never changes the caller's model or small output. */
static int test_lifecycle(void)
{
    UmiDataServer *server = NULL;
    UmiUiWorkspaceCustomisation *model = calloc(1U, sizeof(*model));
    UmiUiWorkspaceCustomisation *before = malloc(sizeof(*before));
    UmiUiWorkspaceCheckpointReport report;
    UmiWorkbenchLayoutChunkStore chunks;
    UmiUiWorkspaceCheckpointScope other_scope = scope;
    char *text = malloc(UMI_UI_LAYOUT_ENCODED_CAPACITY);
    char *first = malloc(UMI_UI_LAYOUT_ENCODED_CAPACITY);
    char *backup = NULL;
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char small[4] = "old";
    UMI_TEST_REQUIRE(model != NULL && before != NULL && text != NULL && first != NULL);
    UMI_TEST_REQUIRE(seed_model(model) == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    *before = *model;
    memcpy(text, "unchanged", sizeof("unchanged"));
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_load_validated(server, &scope,
        model, text, UMI_UI_LAYOUT_ENCODED_CAPACITY, &report) == UMI_STATUS_NOT_FOUND);
    UMI_TEST_REQUIRE(report.storage_revision_known && report.storage_revision == 0U &&
        report.primary_status == UMI_STATUS_NOT_FOUND && !report.durable);
    UMI_TEST_REQUIRE(strcmp(text, "unchanged") == 0 && memcmp(model, before, sizeof(*model)) == 0);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_save(server, &scope, model, 1000U,
        0U, &report) == UMI_STATUS_OK && report.storage_revision == 1U);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_load_validated(server, &scope,
        model, first, UMI_UI_LAYOUT_ENCODED_CAPACITY, &report) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(!report.recovered_last_good && report.saved_at_ns == 1000U &&
        report.storage_revision == 1U && memcmp(model, before, sizeof(*model)) == 0);
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_init(&chunks, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_load(&chunks, backup_id,
        &backup, NULL) == UMI_STATUS_OK && strcmp(first, backup) == 0);
    free(backup); backup = NULL;

    /* A developer keeps the originally observed storage revision after a
     * conflict; another save must not be silently overwritten. */
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_save(server, &scope, model, 2000U,
        0U, &report) == UMI_STATUS_INVALID_STATE);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_load_validated(server, &scope,
        model, small, sizeof(small), NULL) == UMI_STATUS_CAPACITY_EXCEEDED && strcmp(small, "old") == 0);
    other_scope.workspace_id = "another-developer";
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_load_validated(server, &other_scope,
        model, text, UMI_UI_LAYOUT_ENCODED_CAPACITY, &report) == UMI_STATUS_NOT_FOUND);
    other_scope = scope; other_scope.application_id = "org.umicom.other";
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_load_validated(server, &other_scope,
        model, text, UMI_UI_LAYOUT_ENCODED_CAPACITY, NULL) == UMI_STATUS_NOT_FOUND);
    other_scope = scope; other_scope.layout_prefix = "org.umicom.other.";
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_save(server, &other_scope, model,
        2000U, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);

    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_place_canvas_window(model,
        "editor", 0.25, 0.125, 0.50, 0.50) == UMI_STATUS_OK);
    *before = *model;
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_save(server, &scope, model, 2000U,
        1U, NULL) == UMI_STATUS_BUSY);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_restore(server, &scope, model, NULL) == UMI_STATUS_BUSY);
    UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_commit_edit(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_save(server, &scope, model, 2000U,
        1U, &report) == UMI_STATUS_OK && report.storage_revision == 2U);

    /* A valid manifest retains a usable CAS revision even if its chunk is
     * damaged. Recovery selects the previous fully validated saved canvas. */
    UMI_TEST_REQUIRE(record_key(UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK,
        primary_id, 2U, key, sizeof(key)) == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(umi_data_server_set(server, key, "damaged") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_load_validated(server, &scope,
        model, text, UMI_UI_LAYOUT_ENCODED_CAPACITY, &report) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(report.recovered_last_good && report.storage_revision_known &&
        report.storage_revision == 2U && report.primary_status != UMI_STATUS_OK && strcmp(first, text) == 0);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_restore(server, &scope, model, &report) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_active(model)->windows[0].x == 0.125);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_save(server, &scope, model, 3000U,
        2U, &report) == UMI_STATUS_OK && report.storage_revision == 3U);
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_load(&chunks, backup_id,
        &backup, NULL) == UMI_STATUS_OK && strcmp(first, backup) == 0);
    free(backup); backup = NULL;

    /* Unknown manifest metadata cannot authorise overwriting a newer primary.
     * Recovery remains available, but explicit repair is a separate operation. */
    UMI_TEST_REQUIRE(record_key(UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
        primary_id, 0U, key, sizeof(key)) == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(umi_data_server_set(server, key, "invalid manifest") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_load_validated(server, &scope,
        model, text, UMI_UI_LAYOUT_ENCODED_CAPACITY, &report) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(report.recovered_last_good && !report.storage_revision_known && strcmp(first, text) == 0);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_save(server, &scope, model, 4000U,
        3U, NULL) != UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_data_server_get(server, key, text, UMI_UI_LAYOUT_ENCODED_CAPACITY) == UMI_STATUS_OK &&
        strcmp(text, "invalid manifest") == 0);
    UMI_TEST_REQUIRE(umi_data_server_begin(server) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_save(server, &scope, model, 5000U,
        3U, NULL) == UMI_STATUS_BUSY);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_load_validated(server, &scope,
        model, text, UMI_UI_LAYOUT_ENCODED_CAPACITY, NULL) == UMI_STATUS_BUSY);
    UMI_TEST_REQUIRE(umi_data_server_in_transaction(server));
    UMI_TEST_REQUIRE(umi_data_server_rollback(server) == UMI_STATUS_OK);
    free(first); free(text); free(before); free(model);
    umi_data_server_destroy(server);
    return EXIT_SUCCESS;
}

/* Feed malformed but checksum-consistent codec records through the real store.
 * Each rejection preserves the complete live model and caller output bytes. */
static int test_invalid_records(void)
{
    UmiUiWorkspaceCustomisation *model = calloc(1U, sizeof(*model));
    UmiUiWorkspaceCustomisation *before = malloc(sizeof(*before));
    UmiUiLayoutPersistenceRecord *record = calloc(1U, sizeof(*record));
    char *encoded = malloc(UMI_UI_LAYOUT_ENCODED_CAPACITY);
    char *output = malloc(UMI_UI_LAYOUT_ENCODED_CAPACITY);
    size_t index;
    UMI_TEST_REQUIRE(model != NULL && before != NULL && record != NULL && encoded != NULL && output != NULL);
    UMI_TEST_REQUIRE(seed_model(model) == EXIT_SUCCESS);
    *before = *model;
    for (index = 0U; index < 12U; ++index) {
        UmiDataServer *server = NULL;
        UmiWorkbenchLayoutChunkStore chunks;
        memset(record, 0, sizeof(*record));
        record->schema_version = UMI_UI_LAYOUT_PERSISTENCE_SCHEMA_VERSION;
        record->layout = *umi_ui_workspace_customisation_active(model);
        switch (index) {
        case 0U: record->schema_version = 2U; break;
        case 1U: record->layout.windows[0].x = NAN; break;
        case 2U: record->layout.windows[1] = record->layout.windows[0]; record->layout.window_count = 2U; break;
        case 3U: memcpy(record->layout.windows[0].window_id, "editor-2", sizeof("editor-2")); break;
        case 4U: memcpy(record->layout.windows[0].tool_id, "unknown", sizeof("unknown")); break;
        case 5U: memcpy(record->layout.layout_id, "org.umicom.other.custom", sizeof("org.umicom.other.custom")); break;
        case 6U: memcpy(record->layout.windows[0].context_group_id, "missing-context", sizeof("missing-context")); break;
        case 7U: record->layout.revision = UINT64_MAX; break;
        case 8U: record->layout.locked = false; break;
        case 9U: record->layout.name[0] = (char)0xc0; record->layout.name[1] = (char)0xaf; break;
        case 10U: record->layout.windows[0].height = INFINITY; break;
        default: break; /* Append a numeric suffix after the codec's final field. */
        }
        UMI_TEST_REQUIRE(umi_ui_layout_persistence_encode(record, encoded,
            UMI_UI_LAYOUT_ENCODED_CAPACITY) == UMI_STATUS_OK);
        if (index == 11U) {
            const size_t length = strlen(encoded);
            UMI_TEST_REQUIRE(length > 0U && length + 5U < UMI_UI_LAYOUT_ENCODED_CAPACITY);
            memcpy(encoded + length - 1U, "junk\n", sizeof("junk\n"));
        }
        UMI_TEST_REQUIRE(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_init(&chunks, server,
            UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
            UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_save(&chunks, primary_id,
            encoded, 1U, 0U, NULL) == UMI_STATUS_OK);
        memcpy(output, "preserved", sizeof("preserved"));
        UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_load_validated(server, &scope,
            model, output, UMI_UI_LAYOUT_ENCODED_CAPACITY, NULL) != UMI_STATUS_OK);
        UMI_TEST_REQUIRE(strcmp(output, "preserved") == 0);
        UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_restore(server, &scope, model, NULL) != UMI_STATUS_OK);
        UMI_TEST_REQUIRE(memcmp(model, before, sizeof(*model)) == 0);
        umi_data_server_destroy(server);
    }
    /* Raw public C model corruption is checked before the encoder can inspect
     * a nonterminated string or overflow a revision during import validation. */
    {
        UmiDataServer *server = NULL;
        UMI_TEST_REQUIRE(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
        memset(model->active_layout_id, 'x', sizeof(model->active_layout_id));
        UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_save(server, &scope, model, 1U, 0U, NULL) != UMI_STATUS_OK);
        *model = *before; model->revision = UINT64_MAX;
        UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_save(server, &scope, model, 1U, 0U, NULL) == UMI_STATUS_CAPACITY_EXCEEDED);
        UMI_TEST_REQUIRE(umi_data_server_count(server) == 0U);
        umi_data_server_destroy(server);
    }
    free(output); free(encoded); free(record); free(before); free(model);
    return EXIT_SUCCESS;
}

/* A capacity failure after some chunk writes must roll the entire first save
 * back, including the newly seeded last-good copy and its manifest. */
static int test_transaction_capacity(void)
{
    UmiDataServer *server = NULL;
    UmiUiWorkspaceCustomisation *model = calloc(1U, sizeof(*model));
    char key[64], value[32];
    size_t index, before;
    UmiStatus status;
    UMI_TEST_REQUIRE(model != NULL && seed_model(model) == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    for (index = 0U; index < 10000U; ++index) {
        int written = snprintf(key, sizeof(key), "fixture-fill-%zu", index);
        UMI_TEST_REQUIRE(written > 0 && (size_t)written < sizeof(key));
        status = umi_data_server_set(server, key, "owned-test-record");
        if (status == UMI_STATUS_CAPACITY_EXCEEDED) break;
        UMI_TEST_REQUIRE(status == UMI_STATUS_OK);
    }
    UMI_TEST_REQUIRE(index > 0U && index < 10000U);
    UMI_TEST_REQUIRE(snprintf(key, sizeof(key), "fixture-fill-%zu", index - 1U) > 0);
    UMI_TEST_REQUIRE(umi_data_server_delete(server, key) == UMI_STATUS_OK);
    before = umi_data_server_count(server);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_save(server, &scope, model, 1U,
        0U, NULL) == UMI_STATUS_CAPACITY_EXCEEDED);
    UMI_TEST_REQUIRE(!umi_data_server_in_transaction(server) && umi_data_server_count(server) == before);
    UMI_TEST_REQUIRE(umi_data_server_get(server, "fixture-fill-0", value,
        sizeof(value)) == UMI_STATUS_OK && strcmp(value, "owned-test-record") == 0);
    free(model); umi_data_server_destroy(server);
    return EXIT_SUCCESS;
}

/* Malformed manifests must fail before hostile byte counts become allocations,
 * and a manifest copied to another namespace cannot redirect chunk loading. */
static int test_manifest_bounds(void)
{
    UmiDataServer *server = NULL;
    UmiWorkbenchLayoutChunkStore chunks;
    UmiWorkbenchLayoutDataChunkManifest manifest, decoded, original;
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char *text = NULL;
    size_t index;
    UMI_TEST_REQUIRE(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_init(&chunks, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_save(&chunks, primary_id,
        "bounded", 1U, 0U, &original) == UMI_STATUS_OK);
    for (index = 0U; index < 4U; ++index) {
        manifest = original;
        if (index == 0U) manifest.byte_count = SIZE_MAX;
        else if (index == 1U) manifest.chunk_count = 0U;
        else if (index == 2U) manifest.revision = 0U;
        else manifest.chunk_kind = UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK;
        UMI_TEST_REQUIRE(umi_workbench_layout_chunk_manifest_encode(&manifest,
            value, sizeof(value), NULL) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_workbench_layout_chunk_manifest_decode(value, &decoded) != UMI_STATUS_OK);
    }
    manifest = original;
    memcpy(manifest.aggregate_id, backup_id, sizeof(backup_id));
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_manifest_encode(&manifest, value,
        sizeof(value), NULL) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(record_key(UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
        primary_id, 0U, key, sizeof(key)) == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(umi_data_server_set(server, key, value) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_load(&chunks, primary_id,
        &text, NULL) != UMI_STATUS_OK && text == NULL);
    umi_data_server_destroy(server);
    return EXIT_SUCCESS;
}

/* Accept historical fixed-decimal schema3 records while rejecting malformed
 * fields at the codec boundary, before higher-level product policy runs. */
static int test_codec_compatibility(void)
{
    static const char legacy[] =
        "UMILAYOUT3\t3\t42\torg.umicom.fixture.legacy\tLegacy canvas\t1\t1\t1\n"
        "W\teditor\tEditor\teditor\teditor\tcanvas\teditor\t-\t0.12500000\t0.25000000\t0.50000000\t0.50000000\t1\t0\t0\t1\t0\t1\t0\n";
    static const char *invalid[] = {
        "UMILAYOUT3\t3\t42\tid\tName\t2\t0\t1\n",
        "UMILAYOUT3\t3\t42\tid\tName\t1\t0\t1junk\n",
        "UMILAYOUT3\t3\t42\tid\tName\t1\t-1\t1\n",
        "UMILAYOUT3\t3\t18446744073709551616\tid\tName\t1\t0\t1\n",
        "UMILAYOUT3 3\t42\tid\tName\t1\t0\t1\n",
        "UMILAYOUT3\t3\t42\tid\tName\t1\t0\t1\textra\n",
        "UMILAYOUT3\t3\t42\tid\tName\t1\t0\t1\n\n",
        "UMILAYOUT3\t3\t42\tid\tName\t1\t1\t1\n"
        "W\teditor\tEditor\teditor\teditor\tcanvas\teditor\t-\t0\t0\t0.5\t0.5\t2\t0\t0\t1\t0\t1\t0\n",
        "UMILAYOUT3\t3\t42\tid\tName\t1\t1\t1\n"
        "W\teditor\tEditor\teditor\teditor\tcanvas\teditor\t-\t0\t0\t0.5\t0.5\t1\t0\t0\t1\t0\t1\t2147483648\n",
        "UMILAYOUT3\t3\t42\tid\tName\t1\t1\t1\n"
        "W\teditor\tEditor\teditor\teditor\tcanvas\teditor\t-\t1e999\t0\t0.5\t0.5\t1\t0\t0\t1\t0\t1\t0\n"
    };
    UmiDataServer *server = NULL;
    UmiWorkbenchLayoutChunkStore chunks;
    UmiUiWorkspaceCustomisation *model = calloc(1U, sizeof(*model));
    UmiUiLayoutPersistenceRecord *record = malloc(sizeof(*record));
    char *text = malloc(UMI_UI_LAYOUT_ENCODED_CAPACITY);
    size_t index;
    UMI_TEST_REQUIRE(model != NULL && record != NULL && text != NULL && seed_model(model) == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(umi_ui_layout_persistence_decode(legacy, record) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(record->layout.windows[0].x == 0.125 && record->layout.window_count == 1U);
    memcpy(text, legacy, sizeof(legacy));
    text[strlen(text) - 1U] = '\0';
    UMI_TEST_REQUIRE(umi_ui_layout_persistence_decode(text, record) == UMI_STATUS_OK);
    for (index = 0U; index < sizeof(invalid) / sizeof(invalid[0]); ++index)
        UMI_TEST_REQUIRE(umi_ui_layout_persistence_decode(invalid[index], record) == UMI_STATUS_PARSE_ERROR);
    UMI_TEST_REQUIRE(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_init(&chunks, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_save(&chunks, primary_id,
        legacy, 1U, 0U, NULL) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_restore(server, &scope, model, NULL) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(strcmp(model->active_layout_id, "org.umicom.fixture.legacy") == 0);
    free(text); free(record); free(model); umi_data_server_destroy(server);
    return EXIT_SUCCESS;
}

/* A genuinely blank saved canvas is useful state, not a failed layout. Edge
 * geometry must also survive serialization without eight-decimal rounding. */
static int test_blank_and_edge_geometry(void)
{
    UmiDataServer *server = NULL;
    UmiUiWorkspaceCustomisation *model = calloc(1U, sizeof(*model));
    UmiUiWorkspaceCheckpointReport report;
    const double width = 1.0 / 3.0;
    const double x = 1.0 - width;
    UMI_TEST_REQUIRE(model != NULL && seed_model(model) == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_create_blank_layout(model,
        "org.umicom.fixture.empty", "Empty") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_save(server, &scope, model,
        1U, 0U, &report) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_activate(model, "org.umicom.fixture.custom") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_restore(server, &scope, model, &report) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(strcmp(model->active_layout_id, "org.umicom.fixture.empty") == 0 &&
        umi_ui_workspace_customisation_active(model)->window_count == 0U);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_activate(model, "org.umicom.fixture.custom") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_place_canvas_window(model,
        "editor", x, 0.125, width, 0.50) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_commit_edit(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_save(server, &scope, model,
        2U, 1U, &report) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_activate(model, "org.umicom.fixture.empty") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_restore(server, &scope, model, &report) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_active(model)->windows[0].x == x &&
        umi_ui_workspace_customisation_active(model)->windows[0].width == width);
    free(model); umi_data_server_destroy(server);
    return EXIT_SUCCESS;
}

/* SQLite's backend name alone does not prove restart durability. Native host
 * integration fixtures cover disk reopening; this core fixture writes no files. */
static int test_sqlite_memory(void)
{
#ifdef UMICOM_HAS_SQLITE
    UmiDataServer *server = NULL;
    UmiUiWorkspaceCustomisation *model = calloc(1U, sizeof(*model));
    UmiUiWorkspaceCheckpointReport report;
    UMI_TEST_REQUIRE(model != NULL && seed_model(model) == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(umi_data_server_create_sqlite(":memory:", &server) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_save(server, &scope, model,
        1U, 0U, &report) == UMI_STATUS_OK && !report.durable);
    umi_data_server_destroy(server); free(model);
#else
    (void)puts("SQLite memory coverage unavailable: Framework was built without SQLite.");
#endif
    return EXIT_SUCCESS;
}

/* Run portable contracts without launching a desktop window or external tool. */
int main(void)
{
    UMI_TEST_REQUIRE(test_lifecycle() == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(test_invalid_records() == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(test_transaction_capacity() == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(test_manifest_bounds() == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(test_codec_compatibility() == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(test_blank_and_edge_geometry() == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(test_sqlite_memory() == EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
