/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_navigation_workflow.c
 * PURPOSE: Navigate stale compiler locations while preserving edited documents.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostic_ui/navigation.h"
#include "umicom/document/navigation_history.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(value) do { if (!(value)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); failed = 1; goto cleanup; } } while (0)

static UmiStatus AddSource(UmiDiagnosticModel *model, const char *id,
    const char *path, uint32_t line)
{
    UmiDiagnosticSnapshot item;
    UmiStatus status = umi_diagnostic_snapshot_init(&item, id, UMI_DIAGNOSTIC_ERROR,
        UMI_DIAGNOSTIC_KIND_COMPILER, "compiler", "Retained compiler finding");
    if (status != UMI_STATUS_OK) return status;
    if (strlen(path) >= sizeof item.uri) return UMI_STATUS_CAPACITY_EXCEEDED;
    strcpy(item.uri, path);
    item.line = line;
/* A location-free finding must have both line and column unset; retain invalid fixture input for review. Retained for engineering review. */
#if 0
    item.column = 4U;
#endif
    item.column = line == 0U ? 0U : 4U;
    return umi_diagnostic_model_upsert(model, &item);
}

int main(int argc, char **argv)
{
    int failed = 0;
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiDiagnosticModel *model = NULL;
    FILE *file = NULL;
    char *text = NULL;
    size_t length = 0U;
    char source[UMI_PATH_CAPACITY], missing[UMI_PATH_CAPACITY], draftId[UMI_UI_ID_CAPACITY];
    REQUIRE(argc == 3 && umi_path_is_absolute(argv[2]));
    const char *mode = argv[1];
    const char *cases[] = {"next", "previous", "all-missing", "missing-line", "dirty", "no-wrap", "invalid-base", "beyond-page", "history", "history-stale-line", "missing-line-only"};
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(mode, cases[i]) == 0) known = 1;
    REQUIRE(known);
    REQUIRE(umi_fs_join(source, sizeof source, argv[2], "available source.c") == UMI_STATUS_OK);
    REQUIRE(umi_fs_join(missing, sizeof missing, argv[2], "missing source.c") == UMI_STATUS_OK);
    REQUIRE(!umi_fs_exists(missing));
    file = fopen(source, "wb"); REQUIRE(file != NULL);
    REQUIRE(fputs("first\nsecond line\nthird\n", file) >= 0);
    int closed = fclose(file); file = NULL; REQUIRE(closed == 0);
    REQUIRE(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workbench_create("test.available-diagnostic", commands, &workbench) == UMI_STATUS_OK);
    REQUIRE(umi_document_store_create(&store) == UMI_STATUS_OK);
    REQUIRE(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    UmiDiagnosticModelConfig config = {512U};
    REQUIRE(umi_diagnostic_model_create(&config, &model) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    UmiUiDocumentViewSnapshot view;
    if (strcmp(mode, "dirty") == 0)
        REQUIRE(umi_document_coordinator_open(documents, source, draftId, sizeof draftId) == UMI_STATUS_OK);
    else REQUIRE(umi_document_coordinator_new(documents, "Keep my draft", draftId, sizeof draftId) == UMI_STATUS_OK);
    REQUIRE(umi_ui_document_view_model_find(views, draftId, &view) == UMI_STATUS_OK);
    const char *draft = "first\nunsaved source\nthird\n";
    view.dirty = 1; view.cursor_offset = 1U; view.selection_length = 2U;
    REQUIRE(UmiUiDocumentViewModelUpsertText(views, &view, draft, strlen(draft)) == UMI_STATUS_OK);
    if (strcmp(mode, "beyond-page") == 0) {
        for (size_t i = 0U; i < 300U; ++i) {
            char id[64]; snprintf(id, sizeof id, "without-source-%zu", i);
            REQUIRE(AddSource(model, id, "", 0U) == UMI_STATUS_OK);
        }
    }
    int backwards = strcmp(mode, "previous") == 0;
    if (backwards) REQUIRE(AddSource(model, "available", source, 2U) == UMI_STATUS_OK);
    int staleLine = strcmp(mode, "missing-line") == 0 || strcmp(mode, "history-stale-line") == 0 || strcmp(mode, "missing-line-only") == 0;
    const char *stalePath = staleLine ? source : missing;
    if (strcmp(mode, "invalid-base") == 0) stalePath = "available source.c";
    REQUIRE(AddSource(model, "stale", stalePath, staleLine ? 999U : 2U) == UMI_STATUS_OK);
    if (!backwards && strcmp(mode, "all-missing") != 0 && strcmp(mode, "missing-line-only") != 0)
        REQUIRE(AddSource(model, "available", source, 2U) == UMI_STATUS_OK);
    UmiDiagnosticNavigation navigation = {0U, 1};
    if (strcmp(mode, "no-wrap") == 0) {
        UmiDiagnosticSnapshot last;
        REQUIRE(umi_diagnostic_model_find(model, "available", &last) == UMI_STATUS_OK);
        navigation.current_sequence = last.sequence;
        navigation.wrap = 0;
    }
    UmiDocumentNavigationHistorySnapshot history;
    REQUIRE(UmiDocumentCoordinatorNavigationSnapshot(documents, &history) == UMI_STATUS_OK);
    REQUIRE(UmiDocumentCoordinatorClearNavigation(documents, history.revision) == UMI_STATUS_OK);
    UmiDiagnosticNavigation before = navigation;
    UmiDiagnosticFilter filter;
    umi_diagnostic_filter_init(&filter);
    UmiDiagnosticSnapshot selected, original;
    memset(&selected, 0xA5, sizeof selected); original = selected;
    size_t offset = 777U;
    UmiStatus status = UmiDiagnosticNavigateOpenSource(&navigation, model, &filter,
        backwards, documents, strcmp(mode, "invalid-base") == 0 ? NULL : argv[2], &selected, &offset);
    if (strcmp(mode, "all-missing") == 0 || strcmp(mode, "no-wrap") == 0 || strcmp(mode, "invalid-base") == 0 || strcmp(mode, "missing-line-only") == 0) {
        REQUIRE(status == (strcmp(mode, "invalid-base") == 0 ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_NOT_FOUND));
        REQUIRE(navigation.current_sequence == before.current_sequence && navigation.wrap == before.wrap);
        REQUIRE(offset == 777U && memcmp(&selected, &original, sizeof selected) == 0);
        REQUIRE(umi_ui_document_view_model_find(views, draftId, &view) == UMI_STATUS_OK);
        REQUIRE(view.cursor_offset == 1U && view.selection_length == 2U);
        UmiDocumentWorkingCopySnapshot unchanged;
        REQUIRE(umi_document_coordinator_active_snapshot(documents, &unchanged) == UMI_STATUS_OK);
        REQUIRE(strcmp(unchanged.view_id, draftId) == 0);
        REQUIRE(UmiDocumentCoordinatorNavigationSnapshot(documents, &history) == UMI_STATUS_OK && history.count == 0U);
        if (strcmp(mode, "missing-line-only") == 0) REQUIRE(umi_document_coordinator_count(documents) == 2U);
    } else {
        REQUIRE(status == UMI_STATUS_OK && strcmp(selected.id, "available") == 0 && offset == 9U);
        REQUIRE(navigation.current_sequence == selected.sequence);
        UmiDocumentWorkingCopySnapshot active;
        REQUIRE(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK);
        REQUIRE(active.has_path && umi_path_equal(active.path, source));
        REQUIRE(umi_ui_document_view_model_find(views, active.view_id, &view) == UMI_STATUS_OK && view.cursor_offset == 9U);
    }
    if (strcmp(mode, "history") == 0 || strcmp(mode, "history-stale-line") == 0) {
        REQUIRE(UmiDocumentCoordinatorNavigationSnapshot(documents, &history) == UMI_STATUS_OK && history.count == 2U);
        REQUIRE(UmiDocumentCoordinatorTravel(documents, -1, history.revision, NULL) == UMI_STATUS_OK);
        UmiDocumentWorkingCopySnapshot returned;
        REQUIRE(umi_document_coordinator_active_snapshot(documents, &returned) == UMI_STATUS_OK);
        REQUIRE(strcmp(returned.view_id, draftId) == 0 && returned.dirty);
        REQUIRE(umi_ui_document_view_model_find(views, draftId, &view) == UMI_STATUS_OK && view.cursor_offset == 1U);
        REQUIRE(UmiDocumentCoordinatorNavigationSnapshot(documents, &history) == UMI_STATUS_OK);
        REQUIRE(UmiDocumentCoordinatorTravel(documents, 1, history.revision, NULL) == UMI_STATUS_OK);
        REQUIRE(umi_document_coordinator_active_snapshot(documents, &returned) == UMI_STATUS_OK);
        REQUIRE(returned.has_path && umi_path_equal(returned.path, source));
        REQUIRE(umi_ui_document_view_model_find(views, returned.view_id, &view) == UMI_STATUS_OK && view.cursor_offset == 9U);
    }
    REQUIRE(UmiUiDocumentViewModelCopyText(views, draftId, &text, &length) == UMI_STATUS_OK);
    REQUIRE(length == strlen(draft) && memcmp(text, draft, length) == 0);
    REQUIRE(umi_ui_document_view_model_find(views, draftId, &view) == UMI_STATUS_OK && view.dirty);
cleanup:
    if (file != NULL) fclose(file);
    UmiUiDocumentViewModelFreeText(text);
    umi_diagnostic_model_destroy(model);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
