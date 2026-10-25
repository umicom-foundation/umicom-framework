/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/editor_config_adoption_fixture.h
 *
 * PURPOSE:
 *   Share isolated saved-document and configuration evidence fixtures for reviewed adoption.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_CONFIG_ADOPTION_FIXTURE_H
#define UMICOM_EDITOR_CONFIG_ADOPTION_FIXTURE_H
#include "../build_log/fixture.h"
#include "umicom/document/document.h"
#include "umicom/document/editor_config_adoption.h"
#include "umicom/platform/local_replace.h"
#include "umicom/platform/rooted_files.h"
typedef struct AdoptionFixture {
  char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], view[UMI_UI_ID_CAPACITY];
  UmiCommandRegistry *commands;
  UmiUiWorkbench *workbench;
  UmiDocumentStore *store;
  UmiDocumentCoordinator *documents;
  UmiDocumentId id;
  UmiEditorConfigInspection *inspection, *fresh;
  UmiDocumentEditorConfigPlan *plan;
} AdoptionFixture;
static inline void AdoptionDirectory(const char *path) {
#ifdef _WIN32
  wchar_t wide[UMI_PATH_CAPACITY];
  CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide, (int)UMI_PATH_CAPACITY) >
        0);
  CHECK(CreateDirectoryW(wide, NULL));
#else
  CHECK(mkdir(path, 0700) == 0);
#endif
}
static inline void AdoptionSave(AdoptionFixture *f, const char *relative, const char *text) {
  char path[UMI_PATH_CAPACITY];
  FixturePath(path, f->root, relative);
  CHECK(UmiLocalFileReplace(path, text, strlen(text)) == UMI_STATUS_OK);
}
static inline void AdoptionStartFixture(AdoptionFixture *f, const char *config) {
  FixtureDirectory(f->root);
  char child[UMI_PATH_CAPACITY];
  FixturePath(child, f->root, "src");
  AdoptionDirectory(child);
  AdoptionSave(f, "src/main.c", "ab\nc");
  FixturePath(f->path, f->root, "src/main.c");
  if (config)
    AdoptionSave(f, ".editorconfig", config);
  CHECK(umi_command_registry_create(&f->commands) == UMI_STATUS_OK);
  CHECK(umi_ui_workbench_create("editor-config.adoption", f->commands, &f->workbench) ==
        UMI_STATUS_OK);
  CHECK(umi_document_store_create(&f->store) == UMI_STATUS_OK);
  CHECK(umi_document_coordinator_create(f->store, f->workbench, NULL, &f->documents) ==
        UMI_STATUS_OK);
  CHECK(umi_document_coordinator_open(f->documents, f->path, f->view, sizeof f->view) ==
        UMI_STATUS_OK);
  UmiDocumentWorkingCopySnapshot snapshot;
  CHECK(umi_document_coordinator_active_snapshot(f->documents, &snapshot) == UMI_STATUS_OK);
  f->id = snapshot.document_id;
}
static inline void AdoptionInspect(AdoptionFixture *f) {
  CHECK(UmiEditorConfigInspectionLoad(f->root, "src/main.c", NULL, &f->inspection, NULL) ==
        UMI_STATUS_OK);
}
static inline UmiStatus AdoptionPlan(AdoptionFixture *f) {
  UmiStatus status =
      UmiDocumentEditorConfigPlanCreate(f->documents, f->id, f->inspection, &f->plan);
  if (status == UMI_STATUS_OK)
    f->inspection = NULL;
  return status;
}
static inline void AdoptionReinspect(AdoptionFixture *f) {
  CHECK(UmiEditorConfigInspectionLoad(f->root, "src/main.c", NULL, &f->fresh, NULL) ==
        UMI_STATUS_OK);
}
static inline void AdoptionDraft(AdoptionFixture *f, const char *text, int readonly) {
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f->workbench);
  UmiUiDocumentViewSnapshot view;
  CHECK(umi_ui_document_view_model_find(views, f->view, &view) == UMI_STATUS_OK);
  view.dirty = 1;
  view.read_only = readonly;
  view.cursor_offset = 0U;
  view.selection_length = 0U;
  CHECK(UmiUiDocumentViewModelUpsertText(views, &view, text, strlen(text)) == UMI_STATUS_OK);
}
static inline void AdoptionExpectText(AdoptionFixture *f, const char *expected) {
  char *text = NULL;
  size_t bytes = 0U;
  CHECK(UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(f->workbench), f->view, &text,
                                       &bytes) == UMI_STATUS_OK);
  CHECK(bytes == strlen(expected) && !memcmp(text, expected, bytes));
  UmiUiDocumentViewModelFreeText(text);
}
static inline void AdoptionStopFixture(AdoptionFixture *f) {
  UmiDocumentEditorConfigPlanDestroy(f->plan);
  UmiEditorConfigInspectionDestroy(f->inspection);
  UmiEditorConfigInspectionDestroy(f->fresh);
  umi_document_coordinator_destroy(f->documents);
  umi_document_store_destroy(f->store);
  umi_ui_workbench_destroy(f->workbench);
  umi_command_registry_destroy(f->commands);
}
#endif
