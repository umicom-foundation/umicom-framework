/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_text_inspection.c
 *
 * PURPOSE:
 *   Verify document identity and independent read-only inspection across pending edits, closure and
 * owner lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/inspection.h"
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1];
  const char *names[] = {"capture",  "selection", "pending",   "readonly",
                         "inactive", "edited",    "closed",    "owner-retired",
                         "identity", "missing",   "arguments", "history"};
  int known = 0;
  for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
    known |= !strcmp(name, names[i]);
  if (!known)
    return 2;
  ReplacementFixture f = {0};
  CHECK(Start(&f, "a\n\xc3\xa9") == 0);
  UmiDocumentInspection *inspection = NULL;
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
  UmiUiDocumentViewSnapshot view;
  CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
  UmiDocumentEditState before, after;
  CHECK(UmiDocumentCoordinatorGetEditState(f.documents, f.id, &before) == UMI_STATUS_OK);
  if (!strcmp(name, "selection")) {
    view.cursor_offset = 2U;
    view.selection_length = 2U;
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
  }
  if (!strcmp(name, "readonly")) {
    view.read_only = 1;
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
  }
  if (!strcmp(name, "pending"))
    CHECK(Draft(&f, "unsynced") == UMI_STATUS_OK);
  if (!strcmp(name, "identity")) {
    snprintf(view.document_id, sizeof view.document_id, "document.999");
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorInspectText(f.documents, f.id, &inspection) ==
              UMI_STATUS_INVALID_STATE &&
          inspection == NULL);
    Stop(&f);
    return 0;
  }
  if (!strcmp(name, "missing")) {
    CHECK(UmiDocumentCoordinatorInspectText(f.documents, f.id + 100U, &inspection) ==
              UMI_STATUS_NOT_FOUND &&
          inspection == NULL);
    Stop(&f);
    return 0;
  }
  if (!strcmp(name, "arguments")) {
    CHECK(UmiDocumentCoordinatorInspectText(NULL, f.id, &inspection) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorInspectText(f.documents, 0U, &inspection) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorInspectText(f.documents, f.id, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    Stop(&f);
    return 0;
  }
  if (!strcmp(name, "inactive"))
    CHECK(umi_document_coordinator_new(f.documents, "other.c", NULL, 0U) == UMI_STATUS_OK);
  CHECK(UmiDocumentCoordinatorInspectText(f.documents, f.id, &inspection) == UMI_STATUS_OK);
  if (!strcmp(name, "edited"))
    CHECK(Draft(&f, "later") == UMI_STATUS_OK);
  if (!strcmp(name, "closed"))
    CHECK(UmiDocumentCoordinatorClose(f.documents, f.id, 1) == UMI_STATUS_OK);
  if (!strcmp(name, "owner-retired")) {
    Stop(&f);
    memset(&f, 0, sizeof f);
  }
  UmiDocumentInspectionInfo info;
  UmiEditorTextInspectionSummary summary;
  CHECK(UmiDocumentInspectionInfoRead(inspection, &info) == UMI_STATUS_OK);
  CHECK(UmiEditorTextInspectionSummaryRead(UmiDocumentInspectionText(inspection), &summary) ==
        UMI_STATUS_OK);
  const char *text = NULL;
  size_t bytes = 0U;
  CHECK(UmiEditorTextInspectionRead(UmiDocumentInspectionText(inspection), &text, &bytes) ==
        UMI_STATUS_OK);
  CHECK(!strcmp(text, !strcmp(name, "pending") ? "unsynced" : "a\n\xc3\xa9"));
  if (!strcmp(name, "selection"))
    CHECK(summary.selection.scalars == 1U && summary.selection_bytes == 2U);
  if (!strcmp(name, "readonly"))
    CHECK(info.read_only);
  if (!strcmp(name, "inactive")) {
    UmiDocumentWorkingCopySnapshot current;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &current) == UMI_STATUS_OK &&
          current.document_id != info.document_id);
  }
  if (!strcmp(name, "history")) {
    CHECK(UmiDocumentCoordinatorGetEditState(f.documents, f.id, &after) == UMI_STATUS_OK);
    CHECK(after.can_undo == before.can_undo && after.can_redo == before.can_redo &&
          after.selection_offset == before.selection_offset);
  }
  UmiDocumentInspectionDestroy(inspection);
  if (f.documents != NULL)
    Stop(&f);
  return 0;
}
