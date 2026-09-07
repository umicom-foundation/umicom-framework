/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui/test_workspace_library_checkpoint.c
 * PURPOSE: Verify complete named-layout archives, isolated revisions and safe
 * recovery through the existing Data Server without opening desktop windows.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/test_runtime/check.h"
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/customisation.h"
#include "umicom/ui/workspace_library.h"
#include "umicom/ui/workspace_library_checkpoint.h"
#include "umicom/workbench_layout_data/chunk_store.h"
#include "umicom/workbench_layout_data/key_codec.h"

#define FIXTURE_ARCHIVE_CAPACITY (UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS * UMI_UI_LAYOUT_ENCODED_CAPACITY + 2048U)
static const UmiUiWorkspaceCheckpointScope fixture_scope = {
    "org.umicom.fixture", "developer", "org.umicom.fixture."};
static const char primary_id[] = "org.umicom.fixture@developer@library-primary";
static const char backup_id[] = "org.umicom.fixture@developer@library-last-good";
static const char alpha_id[] = "org.umicom.fixture.alpha";
static const char beta_id[] = "org.umicom.fixture.beta";
static const char empty_id[] = "org.umicom.fixture.empty";

/* Build independent named layouts using public C contracts. Context members
 * include a non-layout observer which a restore must never remove. */
static int seed_model(UmiUiWorkspaceCustomisation *model)
{
    UmiUiWindowDescriptor tool = {0};
    UmiUiWorkspaceLayout *active;
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    umi_ui_workspace_customisation_init(model);
    memcpy(tool.tool_id, "editor", sizeof("editor"));
    memcpy(tool.title, "Editor", sizeof("Editor"));
    tool.category = UMI_UI_WINDOW_CATEGORY_DEVELOPMENT;
    tool.default_width = 0.40;
    tool.default_height = 0.50;
    UMI_TEST_REQUIRE(umi_ui_window_catalogue_register(&model->windows, &tool) == UMI_STATUS_OK);
    memcpy(tool.tool_id, "inspector", sizeof("inspector"));
    memcpy(tool.title, "Inspector", sizeof("Inspector"));
    UMI_TEST_REQUIRE(umi_ui_window_catalogue_register(&model->windows, &tool) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_window_group_define(&model->groups, "fixture.context", "green",
        UMI_UI_WINDOW_CONTEXT_FILE) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_window_group_assign(&model->groups, "fixture.context", "outside.observer",
        UMI_UI_WINDOW_GROUP_DESTINATION) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_create_blank_layout(model, alpha_id, "First canvas") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_open_window(model, "editor", "canvas", false,
        1U, window_id, sizeof(window_id)) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_place_canvas_window(model, "editor",
        0.125, 0.25, 0.50, 0.50) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_commit_edit(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_clone_layout(model, alpha_id, beta_id, "Second canvas") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_activate(model, beta_id) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_open_window(model, "inspector", "left", false,
        2U, window_id, sizeof(window_id)) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_set_auto_hidden(model, "inspector", true) == UMI_STATUS_OK);
    active = umi_ui_workspace_customisation_active(model);
    UMI_TEST_REQUIRE(active != NULL && active->window_count == 2U);
    memcpy(active->windows[0].context_group_id, "fixture.context", sizeof("fixture.context"));
    UMI_TEST_REQUIRE(umi_ui_window_group_assign(&model->groups, "fixture.context", "editor",
        UMI_UI_WINDOW_GROUP_SOURCE) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_commit_edit(model) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_create_blank_layout(model, empty_id, "Empty canvas") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_activate(model, beta_id) == UMI_STATUS_OK);
    return EXIT_SUCCESS;
}

/* Exercise the same revision-based library request used by native clients. */
static int apply_library(UmiUiWorkspaceCustomisation *model, UmiUiWorkspaceLibraryAction action,
                         const char *id, const char *name)
{
    const UmiUiWorkspaceLibraryPolicy policy = {"org.umicom.fixture."};
    const UmiUiWorkspaceLibraryRequest request = {action, id, NULL, name, model->revision, true};
    UMI_TEST_REQUIRE(umi_ui_workspace_library_apply(model, &policy, &request, NULL) == UMI_STATUS_OK);
    return EXIT_SUCCESS;
}

/* Build corruption keys through the public namespace codec, not an invented
 * SQL schema or a second private storage service. */
static int damage_record(UmiDataServer *server, UmiWorkbenchLayoutDataRecordKind kind,
                         const char *aggregate, uint64_t revision, const char *value)
{
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    UMI_TEST_REQUIRE(umi_workbench_layout_data_key_build(kind, aggregate, NULL, revision,
        0U, key, sizeof(key)) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_data_server_set(server, key, value) == UMI_STATUS_OK);
    return EXIT_SUCCESS;
}

/* Verify complete-list replacement, deleted defaults, active empty canvases,
 * separate active-checkpoint keys and strict caller-owned transaction rules. */
static int test_lifecycle(void)
{
    UmiDataServer *server = NULL;
    UmiWorkbenchLayoutChunkStore store;
    UmiUiWorkspaceCustomisation *source = calloc(1U, sizeof(*source));
    UmiUiWorkspaceCustomisation *receiver = calloc(1U, sizeof(*receiver));
    UmiUiWorkspaceCustomisation *before = malloc(sizeof(*before));
    UmiUiWorkspaceCustomisation *candidate = malloc(sizeof(*candidate));
    UmiUiWorkspaceCustomisation *output_before = malloc(sizeof(*output_before));
    UmiUiWorkspaceLibraryCheckpointReport report;
    UmiUiWorkspaceCheckpointReport single_report;
    UmiUiWorkspaceCheckpointScope other_scope = fixture_scope;
    UmiUiWindowGroupRole role;
    const UmiUiWindowGroup *group;
    char *first_archive = NULL, *backup = NULL;
    char *single_text = malloc(UMI_UI_LAYOUT_ENCODED_CAPACITY);
    size_t index;
    UMI_TEST_REQUIRE(source != NULL && receiver != NULL && before != NULL && candidate != NULL &&
        output_before != NULL && single_text != NULL);
    UMI_TEST_REQUIRE(seed_model(source) == EXIT_SUCCESS && seed_model(receiver) == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_create_blank_layout(receiver,
        "org.umicom.fixture.local-only", "Not saved") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_activate(receiver, beta_id) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_theme_profile_init(&receiver->theme, "local.theme", "Local theme",
        UMI_UI_THEME_MODE_HIGH_CONTRAST, UMI_UI_DENSITY_SPACIOUS) == UMI_STATUS_OK);
    receiver->layouts[0].revision = 500U;
    receiver->revision = 700U;
    *before = *receiver;
    *candidate = *receiver;
    *output_before = *candidate;
    UMI_TEST_REQUIRE(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_load_candidate(server, &fixture_scope,
        receiver, candidate, &report) == UMI_STATUS_NOT_FOUND);
    UMI_TEST_REQUIRE(report.layout_count == 0U && report.checkpoint.storage_revision_known &&
        report.checkpoint.storage_revision == 0U && report.checkpoint.primary_status == UMI_STATUS_NOT_FOUND &&
        !report.checkpoint.durable);
    UMI_TEST_REQUIRE(memcmp(candidate, output_before, sizeof(*candidate)) == 0);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_save(server, &fixture_scope, source, 50U, 0U,
        &single_report) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_save(server, &fixture_scope, source,
        1000U, 0U, &report) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(report.layout_count == 3U && report.checkpoint.storage_revision == 1U &&
        report.checkpoint.storage_revision_known && !report.checkpoint.durable);
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_init(&store, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_load(&store, primary_id, &first_archive, NULL) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_load(&store, backup_id, &backup, NULL) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(strcmp(first_archive, backup) == 0);
    free(backup); backup = NULL;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_load_candidate(server, &fixture_scope,
        receiver, candidate, &report) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(report.layout_count == 3U && report.checkpoint.saved_at_ns == 1000U &&
        !report.checkpoint.recovered_last_good && strcmp(report.checkpoint.layout_id, beta_id) == 0);
    UMI_TEST_REQUIRE(candidate->layout_count == 3U && strcmp(candidate->active_layout_id, beta_id) == 0 &&
        candidate->revision == 701U && memcmp(receiver, before, sizeof(*receiver)) == 0);
    for (index = 0U; index < source->layout_count; ++index) {
        UMI_TEST_REQUIRE(strcmp(candidate->layouts[index].layout_id, source->layouts[index].layout_id) == 0);
        UMI_TEST_REQUIRE(strcmp(candidate->layouts[index].name, source->layouts[index].name) == 0);
        UMI_TEST_REQUIRE(candidate->layouts[index].revision > 500U && candidate->layouts[index].locked);
        UMI_TEST_REQUIRE(candidate->layouts[index].window_count == source->layouts[index].window_count);
    }
    UMI_TEST_REQUIRE(candidate->layouts[1].windows[0].x == 0.125 &&
        !candidate->layouts[1].windows[1].visible &&
        strcmp(candidate->layouts[1].windows[1].placement_id, "auto-hide:left") == 0);
    UMI_TEST_REQUIRE(memcmp(&candidate->windows, &receiver->windows, sizeof(receiver->windows)) == 0 &&
        memcmp(&candidate->library, &receiver->library, sizeof(receiver->library)) == 0 &&
        memcmp(&candidate->theme, &receiver->theme, sizeof(receiver->theme)) == 0);
    group = umi_ui_window_group_for_window(&candidate->groups, "editor", &role);
    UMI_TEST_REQUIRE(group != NULL && strcmp(group->group_id, "fixture.context") == 0 && role == UMI_UI_WINDOW_GROUP_SOURCE);
    group = umi_ui_window_group_for_window(&candidate->groups, "outside.observer", &role);
    UMI_TEST_REQUIRE(group != NULL && role == UMI_UI_WINDOW_GROUP_DESTINATION);
    {
        const UmiUiWorkspaceLibraryPolicy policy = {"org.umicom.fixture."};
        const UmiUiWorkspaceLibraryRequest stale = {UMI_UI_WORKSPACE_LIBRARY_RENAME,
            beta_id, NULL, "Stale request", receiver->revision, false};
        *output_before = *candidate;
        UMI_TEST_REQUIRE(umi_ui_workspace_library_apply(candidate, &policy, &stale, NULL) == UMI_STATUS_INVALID_STATE);
        UMI_TEST_REQUIRE(memcmp(candidate, output_before, sizeof(*candidate)) == 0);
    }
    other_scope.workspace_id = "another-developer";
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_load_candidate(server, &other_scope,
        receiver, candidate, NULL) == UMI_STATUS_NOT_FOUND);
    other_scope = fixture_scope; other_scope.layout_prefix = "org.umicom.other.";
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_save(server, &other_scope,
        source, 2000U, 1U, NULL) != UMI_STATUS_OK);

    UMI_TEST_REQUIRE(apply_library(source, UMI_UI_WORKSPACE_LIBRARY_REMOVE, alpha_id, NULL) == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(apply_library(source, UMI_UI_WORKSPACE_LIBRARY_RENAME, beta_id,
        "Analysis \xc2\xb7 \xe6\x95\xb0\xe6\x8d\xae") == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(apply_library(source, UMI_UI_WORKSPACE_LIBRARY_ACTIVATE, empty_id, NULL) == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_save(server, &fixture_scope, source,
        2000U, 0U, NULL) == UMI_STATUS_INVALID_STATE);
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_load(&store, primary_id, &backup, NULL) == UMI_STATUS_OK &&
        strcmp(first_archive, backup) == 0);
    free(backup); backup = NULL;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_save(server, &fixture_scope, source,
        2000U, 1U, &report) == UMI_STATUS_OK && report.checkpoint.storage_revision == 2U);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_load_candidate(server, &fixture_scope,
        receiver, candidate, &report) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(candidate->layout_count == 2U && strcmp(candidate->layouts[0].layout_id, beta_id) == 0 &&
        strcmp(candidate->layouts[1].layout_id, empty_id) == 0 && strcmp(candidate->active_layout_id, empty_id) == 0 &&
        strcmp(candidate->layouts[0].name, source->layouts[0].name) == 0 && candidate->layouts[1].window_count == 0U);
    UMI_TEST_REQUIRE(umi_ui_window_group_for_window(&candidate->groups, "editor", NULL) == NULL &&
        umi_ui_window_group_for_window(&candidate->groups, "outside.observer", NULL) != NULL);
    *output_before = *candidate;
    UMI_TEST_REQUIRE(umi_data_server_begin(server) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_save(server, &fixture_scope,
        source, 3000U, 2U, NULL) == UMI_STATUS_BUSY);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_load_candidate(server, &fixture_scope,
        receiver, candidate, NULL) == UMI_STATUS_BUSY);
    UMI_TEST_REQUIRE(umi_data_server_in_transaction(server) && memcmp(candidate, output_before, sizeof(*candidate)) == 0);
    UMI_TEST_REQUIRE(umi_data_server_rollback(server) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(source) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_save(server, &fixture_scope,
        source, 3000U, 2U, NULL) == UMI_STATUS_BUSY);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_load_candidate(server, &fixture_scope,
        source, candidate, NULL) == UMI_STATUS_BUSY);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_cancel_edit(source) == UMI_STATUS_OK);

    /* Only a validated primary may replace last-good; corruption cannot turn
     * a three-layout recovery copy into a partially decoded two-layout list. */
    UMI_TEST_REQUIRE(damage_record(server, UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK,
        primary_id, 2U, "damaged primary") == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_load_candidate(server, &fixture_scope,
        receiver, candidate, &report) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(candidate->layout_count == 3U && strcmp(candidate->active_layout_id, beta_id) == 0 &&
        report.checkpoint.recovered_last_good && report.checkpoint.storage_revision_known &&
        report.checkpoint.storage_revision == 2U && report.checkpoint.primary_status != UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_save(server, &fixture_scope,
        source, 3000U, 2U, &report) == UMI_STATUS_OK && report.checkpoint.storage_revision == 3U);
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_load(&store, backup_id, &backup, NULL) == UMI_STATUS_OK &&
        strcmp(first_archive, backup) == 0);
    free(backup); backup = NULL;
    UMI_TEST_REQUIRE(damage_record(server, UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
        primary_id, 0U, "damaged manifest") == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_load_candidate(server, &fixture_scope,
        receiver, candidate, &report) == UMI_STATUS_OK && report.checkpoint.recovered_last_good &&
        !report.checkpoint.storage_revision_known);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_save(server, &fixture_scope,
        source, 4000U, 3U, NULL) != UMI_STATUS_OK);
    UMI_TEST_REQUIRE(damage_record(server, UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK,
        backup_id, 1U, "damaged backup") == EXIT_SUCCESS);
    *output_before = *candidate;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_load_candidate(server, &fixture_scope,
        receiver, candidate, &report) != UMI_STATUS_OK && report.layout_count == 0U);
    UMI_TEST_REQUIRE(memcmp(candidate, output_before, sizeof(*candidate)) == 0 &&
        memcmp(receiver, before, sizeof(*receiver)) == 0);
    UMI_TEST_REQUIRE(umi_ui_workspace_checkpoint_load_validated(server, &fixture_scope, receiver,
        single_text, UMI_UI_LAYOUT_ENCODED_CAPACITY, &single_report) == UMI_STATUS_OK &&
        single_report.storage_revision == 1U && single_report.saved_at_ns == 50U);
    free(single_text); free(first_archive); free(output_before); free(candidate); free(before);
    free(receiver); free(source); umi_data_server_destroy(server);
    return EXIT_SUCCESS;
}

/* Frame existing codec output with explicit byte lengths. This fixture writer
 * is deliberately independent of production decoding and never writes files. */
static int build_fixture_archive(const UmiUiWorkspaceCustomisation *model, unsigned variant,
                                 char *archive, size_t capacity)
{
    UmiUiLayoutPersistenceRecord *record = calloc(1U, sizeof(*record));
    char *payload = malloc(UMI_UI_LAYOUT_ENCODED_CAPACITY);
    size_t used, index;
    int written;
    UMI_TEST_REQUIRE(record != NULL && payload != NULL);
    written = snprintf(archive, capacity, "UMICOM-WORKSPACE-LIBRARY 1\n%zu\n%zu\n%s\n",
        model->layout_count, strlen(model->active_layout_id), model->active_layout_id);
    UMI_TEST_REQUIRE(written > 0 && (size_t)written < capacity);
    used = (size_t)written;
    for (index = 0U; index < model->layout_count; ++index) {
        size_t length;
        record->schema_version = variant == 11U && index == 0U ? 2U : UMI_UI_LAYOUT_PERSISTENCE_SCHEMA_VERSION;
        record->saved_at_ns = variant == 12U && index == 1U ? 43U : 42U;
        record->layout = model->layouts[index];
        UMI_TEST_REQUIRE(umi_ui_layout_persistence_encode(record, payload, UMI_UI_LAYOUT_ENCODED_CAPACITY) == UMI_STATUS_OK);
        length = strlen(payload);
        written = snprintf(archive + used, capacity - used, "%zu\n", length);
        UMI_TEST_REQUIRE(written > 0 && (size_t)written < capacity - used);
        used += (size_t)written;
        UMI_TEST_REQUIRE(length + 2U <= capacity - used);
        memcpy(archive + used, payload, length); used += length;
        archive[used++] = '\n'; archive[used] = '\0';
    }
    if (variant == 13U) {
        UMI_TEST_REQUIRE(used + sizeof("trailing") <= capacity);
        memcpy(archive + used, "trailing", sizeof("trailing"));
    }
    free(payload); free(record);
    return EXIT_SUCCESS;
}

/* Reject checksum-valid bad archives, including malformed inactive and hidden
 * panels, without publishing even the first valid record into the receiver. */
static int test_invalid_archives(void)
{
    static const char *const bad_framing[] = {
        "UMICOM-WORKSPACE-LIBRARY 1\n0\n1\nx\n",
        "UMICOM-WORKSPACE-LIBRARY 1\n17\n1\nx\n",
        "UMICOM-WORKSPACE-LIBRARY 1\n18446744073709551616\n1\nx\n",
        "UMICOM-WORKSPACE-LIBRARY 1\n1junk\n1\nx\n",
        "UMICOM-WORKSPACE-LIBRARY 1\n1\n999999999999999999999\nx\n",
        "UMICOM-WORKSPACE-LIBRARY 1\n1\n1\nx\n18446744073709551616\n",
        "UMICOM-WORKSPACE-LIBRARY 2\n1\n1\nx\n1\na\n"
    };
    UmiUiWorkspaceCustomisation *model = calloc(1U, sizeof(*model));
    UmiUiWorkspaceCustomisation *invalid = malloc(sizeof(*invalid));
    UmiUiWorkspaceCustomisation *candidate = malloc(sizeof(*candidate));
    UmiUiWorkspaceCustomisation *before = malloc(sizeof(*before));
    char *archive = malloc(FIXTURE_ARCHIVE_CAPACITY);
    unsigned variant;
    UMI_TEST_REQUIRE(model != NULL && invalid != NULL && candidate != NULL && before != NULL && archive != NULL);
    UMI_TEST_REQUIRE(seed_model(model) == EXIT_SUCCESS);
    model->windows.items[0].supports_multiple = true;
    *before = *model;
    for (variant = 0U; variant < 15U + sizeof(bad_framing) / sizeof(bad_framing[0]); ++variant) {
        UmiDataServer *server = NULL;
        UmiWorkbenchLayoutChunkStore store;
        UmiUiWorkspaceLibraryCheckpointReport report;
        UmiStatus load_status;
        *invalid = *model; *candidate = *before;
        switch (variant) {
        case 0U: memcpy(invalid->layouts[1].layout_id, alpha_id, sizeof(alpha_id)); break;
        case 1U: memcpy(invalid->active_layout_id, "org.umicom.fixture.absent", sizeof("org.umicom.fixture.absent")); break;
        case 2U: memcpy(invalid->layouts[1].windows[1].tool_id, "unknown", sizeof("unknown")); break;
        case 3U: memcpy(invalid->layouts[1].windows[1].context_group_id, "unknown", sizeof("unknown")); break;
        case 4U: memcpy(invalid->layouts[0].layout_id, "org.umicom.foreign.alpha", sizeof("org.umicom.foreign.alpha")); break;
        case 5U: memcpy(invalid->layouts[1].windows[1].window_id, "editor", sizeof("editor")); break;
        case 6U: invalid->layouts[0].windows[0].x = NAN; break;
        case 7U: invalid->layouts[0].locked = false; break;
        case 8U: invalid->layouts[0].name[0] = (char)0xc0; invalid->layouts[0].name[1] = (char)0xaf; break;
        case 9U: invalid->layouts[0].revision = UINT64_MAX; break;
        case 10U: invalid->layouts[1].windows[1].visible = true; break;
        case 14U:
            /* The active beta layout remains small. An inactive alpha layout
             * must also be usable later without overflowing context routing. */
            UMI_TEST_REQUIRE(UMI_UI_WINDOW_GROUP_MAX_MEMBERS + 1U <= UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS);
            invalid->layouts[0].window_count = UMI_UI_WINDOW_GROUP_MAX_MEMBERS + 1U;
            for (size_t member = 0U; member < invalid->layouts[0].window_count; ++member) {
                UmiUiWorkspaceWindow *window = &invalid->layouts[0].windows[member];
                int written;
                *window = model->layouts[0].windows[0];
                written = snprintf(window->window_id, sizeof(window->window_id), "editor-%zu", member);
                UMI_TEST_REQUIRE(written > 0 && (size_t)written < sizeof(window->window_id));
                memcpy(window->group_id, window->window_id, sizeof(window->group_id));
                memcpy(window->stack_id, window->window_id, sizeof(window->stack_id));
                memcpy(window->context_group_id, "fixture.context", sizeof("fixture.context"));
            }
            break;
        default: break;
        }
        if (variant < 15U) {
            UMI_TEST_REQUIRE(build_fixture_archive(invalid, variant, archive, FIXTURE_ARCHIVE_CAPACITY) == EXIT_SUCCESS);
        } else {
            const char *text = bad_framing[variant - 15U];
            memcpy(archive, text, strlen(text) + 1U);
        }
        UMI_TEST_REQUIRE(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_init(&store, server,
            UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
            UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_save(&store, primary_id, archive,
            1U, 0U, NULL) == UMI_STATUS_OK);
        load_status = umi_ui_workspace_library_checkpoint_load_candidate(server, &fixture_scope,
            model, candidate, &report);
        UMI_TEST_REQUIRE(load_status != UMI_STATUS_OK);
        if (variant == 14U) UMI_TEST_REQUIRE(load_status == UMI_STATUS_CAPACITY_EXCEEDED);
        UMI_TEST_REQUIRE(report.layout_count == 0U && memcmp(candidate, before, sizeof(*candidate)) == 0 &&
            memcmp(model, before, sizeof(*model)) == 0);
        umi_data_server_destroy(server);
    }
    free(archive); free(before); free(candidate); free(invalid); free(model);
    return EXIT_SUCCESS;
}

/* A missing primary does not make existing but unusable recovery data an empty
 * store. Keep trustworthy primary CAS zero separate from the overall error. */
static int test_backup_only_errors(void)
{
    UmiUiWorkspaceCustomisation *model = calloc(1U, sizeof(*model));
    UmiUiWorkspaceCustomisation *invalid = malloc(sizeof(*invalid));
    UmiUiWorkspaceCustomisation *candidate = malloc(sizeof(*candidate));
    UmiUiWorkspaceCustomisation *before = malloc(sizeof(*before));
    char *archive = malloc(FIXTURE_ARCHIVE_CAPACITY);
    unsigned variant;
    UMI_TEST_REQUIRE(model != NULL && invalid != NULL && candidate != NULL && before != NULL && archive != NULL);
    UMI_TEST_REQUIRE(seed_model(model) == EXIT_SUCCESS);
    *before = *model;
    for (variant = 0U; variant < 4U; ++variant) {
        UmiDataServer *server = NULL;
        UmiWorkbenchLayoutChunkStore store;
        UmiUiWorkspaceLibraryCheckpointReport report;
        UmiStatus status;
        *invalid = *model; *candidate = *before;
        if (variant == 0U)
            memcpy(invalid->layouts[1].windows[1].tool_id, "unknown", sizeof("unknown"));
        else if (variant == 1U)
            memcpy(invalid->layouts[1].windows[1].context_group_id, "unknown", sizeof("unknown"));
        UMI_TEST_REQUIRE(build_fixture_archive(invalid, 0U, archive, FIXTURE_ARCHIVE_CAPACITY) == EXIT_SUCCESS);
        UMI_TEST_REQUIRE(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_init(&store, server,
            UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
            UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_save(&store, backup_id, archive,
            1U, 0U, NULL) == UMI_STATUS_OK);
        if (variant == 2U) {
            char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
            UMI_TEST_REQUIRE(umi_workbench_layout_data_key_build(
                UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK, backup_id, NULL, 0U, 0U,
                key, sizeof(key)) == UMI_STATUS_OK);
            UMI_TEST_REQUIRE(umi_data_server_delete(server, key) == UMI_STATUS_OK);
        } else if (variant == 3U) {
            UMI_TEST_REQUIRE(damage_record(server, UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
                backup_id, 0U, "invalid backup manifest") == EXIT_SUCCESS);
        }
        status = umi_ui_workspace_library_checkpoint_load_candidate(server, &fixture_scope,
            model, candidate, &report);
        UMI_TEST_REQUIRE(status == (variant < 2U ? UMI_STATUS_INVALID_STATE : UMI_STATUS_PARSE_ERROR));
        UMI_TEST_REQUIRE(report.checkpoint.primary_status == UMI_STATUS_NOT_FOUND &&
            report.checkpoint.storage_revision_known && report.checkpoint.storage_revision == 0U &&
            report.layout_count == 0U && !report.checkpoint.recovered_last_good);
        UMI_TEST_REQUIRE(memcmp(candidate, before, sizeof(*candidate)) == 0 &&
            memcmp(model, before, sizeof(*model)) == 0 && !umi_data_server_in_transaction(server));
        umi_data_server_destroy(server);
    }
    free(archive); free(before); free(candidate); free(invalid); free(model);
    return EXIT_SUCCESS;
}

/* Reject destination overlap and revision exhaustion before any live copy can
 * be changed. A transaction-capacity failure must restore both saved slots. */
static int test_atomic_failures(void)
{
    UmiDataServer *server = NULL;
    UmiWorkbenchLayoutChunkStore store;
    UmiUiWorkspaceCustomisation *model = calloc(1U, sizeof(*model));
    UmiUiWorkspaceCustomisation *before = malloc(sizeof(*before));
    UmiUiWorkspaceCustomisation *candidate = malloc(sizeof(*candidate));
    char *original = NULL, *after = NULL;
    char key[64U];
    size_t index;
    UmiStatus status;
    UMI_TEST_REQUIRE(model != NULL && before != NULL && candidate != NULL && seed_model(model) == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_save(server, &fixture_scope, model,
        1U, 0U, NULL) == UMI_STATUS_OK);
    *before = *model; *candidate = *model;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_load_candidate(server, &fixture_scope,
        model, model, NULL) == UMI_STATUS_INVALID_ARGUMENT && memcmp(model, before, sizeof(*model)) == 0);
    model->revision = UINT64_MAX;
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_load_candidate(server, &fixture_scope,
        model, candidate, NULL) != UMI_STATUS_OK && memcmp(candidate, before, sizeof(*candidate)) == 0);
    *model = *before;
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_init(&store, server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_MANIFEST,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_WORKSPACE_CHUNK) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_load(&store, primary_id, &original, NULL) == UMI_STATUS_OK);
    /* Revisions reuse existing chunk keys. Grow the candidate beyond the old
     * chunk count so a full server fails after updating an existing chunk. */
    while (model->layout_count < UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS) {
        const int written = snprintf(key, sizeof(key), "org.umicom.fixture.copy-%zu", model->layout_count);
        UMI_TEST_REQUIRE(written > 0 && (size_t)written < sizeof(key));
        UMI_TEST_REQUIRE(umi_ui_workspace_customisation_clone_layout(model, beta_id, key,
            "Additional saved canvas") == UMI_STATUS_OK);
    }
    {
        UmiDataServer *capacity_check = NULL;
        /* Prove the sixteen-layout candidate itself is valid before the full
         * server forces a storage failure, rather than a validation failure. */
        UMI_TEST_REQUIRE(umi_data_server_create_memory(&capacity_check) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_save(capacity_check, &fixture_scope,
            model, 2U, 0U, NULL) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_load_candidate(capacity_check, &fixture_scope,
            model, candidate, NULL) == UMI_STATUS_OK && candidate->layout_count == UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS);
        umi_data_server_destroy(capacity_check);
    }
    for (index = 0U; index < 4096U; ++index) {
        const int written = snprintf(key, sizeof(key), "fixture.fill.%zu", index);
        UMI_TEST_REQUIRE(written > 0 && (size_t)written < sizeof(key));
        status = umi_data_server_set(server, key, "occupied");
        if (status == UMI_STATUS_CAPACITY_EXCEEDED) break;
        UMI_TEST_REQUIRE(status == UMI_STATUS_OK);
    }
    UMI_TEST_REQUIRE(index < 4096U);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_save(server, &fixture_scope, model,
        2U, 1U, NULL) == UMI_STATUS_CAPACITY_EXCEEDED && !umi_data_server_in_transaction(server));
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_load(&store, primary_id, &after, NULL) == UMI_STATUS_OK &&
        strcmp(original, after) == 0);
    free(after); after = NULL;
    UMI_TEST_REQUIRE(umi_workbench_layout_chunk_store_load(&store, backup_id, &after, NULL) == UMI_STATUS_OK &&
        strcmp(original, after) == 0);
    free(after); free(original); free(candidate); free(before); free(model); umi_data_server_destroy(server);
    return EXIT_SUCCESS;
}

/* SQLite's in-memory backend verifies the same service path without claiming
 * disk-reopen durability. Native integration fixtures own file-based coverage. */
static int test_sqlite_memory(void)
{
#ifdef UMICOM_HAS_SQLITE
    UmiDataServer *server = NULL;
    UmiUiWorkspaceCustomisation *model = calloc(1U, sizeof(*model));
    UmiUiWorkspaceCustomisation *candidate = malloc(sizeof(*candidate));
    UmiUiWorkspaceLibraryCheckpointReport report;
    UMI_TEST_REQUIRE(model != NULL && candidate != NULL && seed_model(model) == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(umi_data_server_create_sqlite(":memory:", &server) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_save(server, &fixture_scope, model,
        1U, 0U, &report) == UMI_STATUS_OK && !report.checkpoint.durable);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_load_candidate(server, &fixture_scope,
        model, candidate, &report) == UMI_STATUS_OK && candidate->layout_count == 3U &&
        !report.checkpoint.durable && strcmp(candidate->active_layout_id, beta_id) == 0);
    free(candidate); free(model); umi_data_server_destroy(server);
#else
    (void)puts("SQLite library archive coverage unavailable: Framework was built without SQLite.");
#endif
    return EXIT_SUCCESS;
}

/* Every current product starts from Framework-owned stock metadata. Verify
 * those actual defaults satisfy the same strict archive rules as user layouts;
 * this is a portable contract check, not an application launch or UI test. */
static int test_all_product_defaults(void)
{
    UmiUiWorkspaceCustomisation *model = calloc(1U, sizeof(*model));
    UmiUiWorkspaceCustomisation *receiver = calloc(1U, sizeof(*receiver));
    UmiUiWorkspaceCustomisation *candidate = malloc(sizeof(*candidate));
    const size_t count = umi_application_experience_catalogue_count();
    size_t product, layout;
    UMI_TEST_REQUIRE(model != NULL && receiver != NULL && candidate != NULL && count >= 24U);
    for (product = 0U; product < count; ++product) {
        const UmiApplicationExperienceDefinition *experience = umi_application_experience_catalogue_at(product);
        UmiUiWorkspaceLibraryCheckpointReport report;
        UmiUiWorkspaceCheckpointScope scope;
        UmiDataServer *server = NULL;
        char prefix[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
        int written;
        UMI_TEST_REQUIRE(experience != NULL && experience->application_id != NULL);
        (void)printf("Checking named-library defaults: %s\n", experience->application_id);
        written = snprintf(prefix, sizeof(prefix), "%s.", experience->application_id);
        UMI_TEST_REQUIRE(written > 0 && (size_t)written < sizeof(prefix));
        scope.application_id = experience->application_id;
        scope.workspace_id = "defaults-fixture";
        scope.layout_prefix = prefix;
        UMI_TEST_REQUIRE(umi_application_suite_customisation_load_experience(model, experience) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_application_suite_customisation_load_experience(receiver, experience) == UMI_STATUS_OK);
        for (layout = 0U; layout < model->layout_count; ++layout) {
            for (size_t window = 0U; window < model->layouts[layout].window_count; ++window) {
                const UmiUiWorkspaceWindow *record = &model->layouts[layout].windows[window];
                size_t panel;
                for (panel = 0U; panel < experience->panel_count; ++panel)
                    if (strcmp(experience->panels[panel].panel_id, record->tool_id) == 0) break;
                UMI_TEST_REQUIRE(panel < experience->panel_count);
                if ((experience->panels[panel].flags & UMI_EXPERIENCE_PANEL_CONTEXT_LINKED) == 0U)
                    UMI_TEST_REQUIRE(record->context_group_id[0] == '\0');
            }
        }
        UMI_TEST_REQUIRE(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_save(server, &scope, model,
            42U, 0U, &report) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(report.layout_count == model->layout_count && report.checkpoint.storage_revision == 1U);
        UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_load_candidate(server, &scope,
            receiver, candidate, &report) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(candidate->layout_count == model->layout_count &&
            strcmp(candidate->active_layout_id, model->active_layout_id) == 0);
        for (layout = 0U; layout < model->layout_count; ++layout) {
            UMI_TEST_REQUIRE(strcmp(candidate->layouts[layout].layout_id, model->layouts[layout].layout_id) == 0 &&
                strcmp(candidate->layouts[layout].name, model->layouts[layout].name) == 0 &&
                candidate->layouts[layout].window_count == model->layouts[layout].window_count);
        }
        umi_data_server_destroy(server);
    }
    free(candidate); free(receiver); free(model);
    return EXIT_SUCCESS;
}

/* Keep checks active in release builds and return ordinary diagnostic codes. */
int main(void)
{
    UMI_TEST_REQUIRE(test_lifecycle() == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(test_invalid_archives() == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(test_backup_only_errors() == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(test_atomic_failures() == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(test_sqlite_memory() == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(test_all_product_defaults() == EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
