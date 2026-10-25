/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_editor_config_adoption.c
 *
 * PURPOSE:
 *   Verify reviewed configuration adoption preserves document identity, source and session policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "editor_config_adoption_fixture.h"
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1],
             *names[] = {"proposal",          "apply",          "style-only",     "tab-size",
                         "unrepresentable",   "large-width",    "invalid-style",  "invalid-size",
                         "invalid-tab-width", "missing",        "unset",          "nonindent",
                         "unknown",           "invalid-other",  "stale-file",     "closer-added",
                         "removed",           "comment-change", "stale-default",  "stale-override",
                         "save-as",           "closed",         "other-document", "draft",
                         "readonly",          "repeated",       "ownership",      "arguments",
                         "source-metadata",   "untitled",       "foreign-owner"};
  int known = 0;
  for (size_t i = 0; i < sizeof names / sizeof names[0]; ++i)
    if (!strcmp(name, names[i]))
      known = 1;
  if (!known)
    return 2;
  const char *config = "root=true\n[*]\nindent_size=2\nindent_style=space\n";
  UmiStatus expected = UMI_STATUS_OK;
  unsigned width = 2U;
  int spaces = 1;
  if (!strcmp(name, "style-only")) {
    config = "[*]\nindent_style=tab\n";
    width = 4U;
    spaces = 0;
  }
  if (!strcmp(name, "tab-size")) {
    config = "[*]\nindent_style=tab\nindent_size=tab\ntab_width=8\n";
    width = 8U;
    spaces = 0;
  }
  if (!strcmp(name, "unrepresentable")) {
    config = "[*]\nindent_size=2\ntab_width=4\n";
    expected = UMI_STATUS_NOT_IMPLEMENTED;
  }
  if (!strcmp(name, "large-width")) {
    config = "[*]\nindent_size=100\n";
    expected = UMI_STATUS_NOT_IMPLEMENTED;
  }
  if (!strcmp(name, "invalid-style")) {
    config = "[*]\nindent_style=invalid\n";
    expected = UMI_STATUS_PARSE_ERROR;
  }
  if (!strcmp(name, "invalid-size")) {
    config = "[*]\nindent_size=0\n";
    expected = UMI_STATUS_PARSE_ERROR;
  }
  if (!strcmp(name, "invalid-tab-width")) {
    config = "[*]\ntab_width=oops\n";
    expected = UMI_STATUS_PARSE_ERROR;
  }
  if (!strcmp(name, "missing")) {
    config = NULL;
    expected = UMI_STATUS_NOT_FOUND;
  }
  if (!strcmp(name, "unset")) {
    config = "[*]\nindent_size=unset\n";
    expected = UMI_STATUS_NOT_FOUND;
  }
  if (!strcmp(name, "nonindent")) {
    config = "[*]\ncharset=utf-8\n";
    expected = UMI_STATUS_NOT_FOUND;
  }
  if (!strcmp(name, "unknown"))
    config = "[*]\nindent_size=2\ncustom_option=keep\n";
  if (!strcmp(name, "invalid-other"))
    config = "[*]\nindent_size=2\ncharset=invalid\n";
  AdoptionFixture f = {0};
  AdoptionStartFixture(&f, config);
  AdoptionInspect(&f);
  if (!strcmp(name, "untitled")) {
    CHECK(umi_document_coordinator_close_active(f.documents, 1) == UMI_STATUS_OK);
    char view[UMI_UI_ID_CAPACITY];
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_new(f.documents, "main.c", view, sizeof view) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK);
    f.id = active.document_id;
    expected = UMI_STATUS_INVALID_STATE;
  }
  if (!strcmp(name, "arguments")) {
    CHECK(UmiDocumentEditorConfigPlanCreate(NULL, f.id, f.inspection, &f.plan) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          !f.plan);
    CHECK(UmiDocumentEditorConfigPlanCreate(f.documents, 0U, f.inspection, &f.plan) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          !f.plan);
    CHECK(UmiDocumentEditorConfigPlanCreate(f.documents, f.id, NULL, &f.plan) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          !f.plan);
    CHECK(UmiDocumentEditorConfigPlanCreate(f.documents, f.id, f.inspection, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentEditorConfigPlanCheck(NULL, f.documents) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentEditorConfigPlanApplyObserved(NULL, f.documents, f.inspection) ==
          UMI_STATUS_INVALID_ARGUMENT);
    AdoptionStopFixture(&f);
    return 0;
  }
  CHECK(AdoptionPlan(&f) == expected);
  if (expected != UMI_STATUS_OK) {
    CHECK(f.plan == NULL && f.inspection != NULL);
    AdoptionStopFixture(&f);
    return 0;
  }
  UmiDocumentEditorConfigSummary summary;
  CHECK(UmiDocumentEditorConfigPlanInspect(f.plan, &summary) == UMI_STATUS_OK);
  CHECK(summary.document_id == f.id && summary.previous.tab_width == 4U &&
        summary.proposed.tab_width == width && summary.proposed.insert_spaces == spaces &&
        !summary.applied);
  if (!strcmp(name, "unknown"))
    CHECK(summary.ignored_properties == 1U && summary.unknown_properties == 1U);
  if (!strcmp(name, "invalid-other"))
    CHECK(summary.ignored_properties == 1U && summary.invalid_values == 1U);
  if (!strcmp(name, "source-metadata")) {
    UmiEditorConfigInspectionFile source;
    CHECK(UmiDocumentEditorConfigPlanSource(f.plan, 0U, &source) == UMI_STATUS_OK &&
          !strcmp(source.relative_path, ".editorconfig") && strlen(source.sha256) == 64U);
    CHECK(UmiDocumentEditorConfigPlanSource(f.plan, 1U, &source) == UMI_STATUS_NOT_FOUND);
  }
  if (!strcmp(name, "proposal") || !strcmp(name, "ownership") || !strcmp(name, "source-metadata")) {
    CHECK(f.inspection == NULL);
    AdoptionExpectText(&f, "ab\nc");
    AdoptionStopFixture(&f);
    return 0;
  }
  const char *text = "ab\nc";
  if (!strcmp(name, "stale-file") || !strcmp(name, "comment-change")) {
    AdoptionSave(&f, ".editorconfig",
                 !strcmp(name, "stale-file")
                     ? "[*]\nindent_size=8\n"
                     : "# comment\nroot=true\n[*]\nindent_size=2\nindent_style=space\n");
    expected = UMI_STATUS_INVALID_STATE;
  }
  if (!strcmp(name, "closer-added")) {
    AdoptionSave(&f, "src/.editorconfig", "[*]\nindent_size=2\n");
    expected = UMI_STATUS_INVALID_STATE;
  }
  if (!strcmp(name, "removed")) {
    CHECK(UmiRootedFileRemove(f.root, ".editorconfig") == UMI_STATUS_OK);
    expected = UMI_STATUS_INVALID_STATE;
  }
  if (!strcmp(name, "stale-default")) {
    UmiEditorIndentation next = {8U, 0};
    CHECK(UmiDocumentCoordinatorSetIndentation(f.documents, &next) == UMI_STATUS_OK);
    expected = UMI_STATUS_INVALID_STATE;
  }
  if (!strcmp(name, "stale-override")) {
    UmiDocumentIndentationReview review;
    UmiEditorIndentation next = {8U, 0};
    CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, f.id, &review) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &review, &next, NULL) ==
          UMI_STATUS_OK);
    expected = UMI_STATUS_INVALID_STATE;
  }
  if (!strcmp(name, "save-as")) {
    char destination[UMI_PATH_CAPACITY];
    FixturePath(destination, f.root, "renamed.c");
    CHECK(UmiDocumentCoordinatorSaveAs(f.documents, f.id, destination) == UMI_STATUS_OK);
    expected = UMI_STATUS_INVALID_STATE;
  }
  if (!strcmp(name, "closed")) {
    CHECK(UmiDocumentCoordinatorClose(f.documents, f.id, 1) == UMI_STATUS_OK);
    expected = UMI_STATUS_NOT_FOUND;
  }
  UmiDocumentId other = 0U;
  if (!strcmp(name, "other-document")) {
    char id[UMI_UI_ID_CAPACITY];
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_new(f.documents, "peer.c", id, sizeof id) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK);
    other = active.document_id;
  }
  if (!strcmp(name, "draft")) {
    text = "new draft";
    AdoptionDraft(&f, text, 0);
  }
  if (!strcmp(name, "readonly"))
    AdoptionDraft(&f, text, 1);
  AdoptionReinspect(&f);
  if (!strcmp(name, "foreign-owner")) {
    AdoptionFixture peer = {0};
    AdoptionStartFixture(&peer, config);
    CHECK(UmiDocumentEditorConfigPlanApplyObserved(f.plan, peer.documents, f.fresh) ==
          UMI_STATUS_INVALID_STATE);
    AdoptionStopFixture(&peer);
    expected = UMI_STATUS_OK;
  }
  CHECK(UmiDocumentEditorConfigPlanApplyObserved(f.plan, f.documents, f.fresh) == expected);
  CHECK(UmiDocumentEditorConfigPlanInspect(f.plan, &summary) == UMI_STATUS_OK &&
        summary.applied == (expected == UMI_STATUS_OK));
  if (expected == UMI_STATUS_OK) {
    UmiDocumentIndentationReview review;
    CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, f.id, &review) == UMI_STATUS_OK &&
          review.overridden && review.effective.tab_width == width &&
          review.effective.insert_spaces == spaces);
    if (!strcmp(name, "repeated"))
      CHECK(UmiDocumentEditorConfigPlanApplyObserved(f.plan, f.documents, f.fresh) ==
            UMI_STATUS_ALREADY_EXISTS);
    if (other) {
      CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, other, &review) ==
                UMI_STATUS_OK &&
            !review.overridden);
    }
  }
  if (strcmp(name, "closed"))
    AdoptionExpectText(&f, text);
  AdoptionStopFixture(&f);
  return 0;
}
