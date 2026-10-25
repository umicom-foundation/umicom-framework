/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_language_override.c
 *
 * PURPOSE:
 *   Verify stale-language reviews, owner identity, policy restoration and refusal atomicity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/language_override.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1], *cases[] = {"inspect",
                                          "apply",
                                          "clear",
                                          "same-detected",
                                          "noop",
                                          "automatic-noop",
                                          "aba",
                                          "foreign-owner",
                                          "foreign-view",
                                          "tampered-id",
                                          "tampered-name",
                                          "tampered-effective",
                                          "tampered-detected",
                                          "tampered-override",
                                          "unterminated",
                                          "closed",
                                          "invalid-choice",
                                          "null-owner",
                                          "null-review",
                                          "null-output",
                                          "zero-id",
                                          "alias",
                                          "inactive"};
  int known = 0;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    known |= !strcmp(name, cases[i]);
  CHECK(known);
  ReplacementFixture f = {0};
  CHECK(Start(&f, "alpha\nbeta") == 0);
  UmiDocumentLanguageReview before, current, guard;
  memset(&guard, 0x5a, sizeof guard);
  current = guard;
  CHECK(UmiDocumentCoordinatorInspectLanguage(f.documents, f.id, &before) == UMI_STATUS_OK);
  CHECK(!before.overridden && !strcmp(before.effective.language_id, "c"));
  if (!strcmp(name, "inspect"))
    CHECK(UmiDocumentCoordinatorCheckLanguage(f.documents, &before) == UMI_STATUS_OK);
  else if (!strcmp(name, "apply") || !strcmp(name, "alias")) {
    current = before;
    CHECK(UmiDocumentCoordinatorApplyLanguage(f.documents,
                                              !strcmp(name, "alias") ? &current : &before,
                                              "python3", &current) == UMI_STATUS_OK);
    CHECK(current.overridden && !strcmp(current.effective.language_id, "python3") &&
          current.revision == before.revision + 1U);
  } else if (!strcmp(name, "clear")) {
    CHECK(UmiDocumentCoordinatorApplyLanguage(f.documents, &before, "sql", &current) ==
          UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorApplyLanguage(f.documents, &current, NULL, &current) ==
          UMI_STATUS_OK);
    CHECK(!current.overridden && !strcmp(current.effective.language_id, "c") &&
          current.revision == before.revision + 2U);
  } else if (!strcmp(name, "same-detected")) {
    CHECK(UmiDocumentCoordinatorApplyLanguage(f.documents, &before, "c", &current) ==
          UMI_STATUS_OK);
    CHECK(current.overridden && current.revision == before.revision + 1U);
  } else if (!strcmp(name, "noop")) {
    CHECK(UmiDocumentCoordinatorApplyLanguage(f.documents, &before, "python3", &current) ==
          UMI_STATUS_OK);
    uint64_t revision = current.revision;
    CHECK(UmiDocumentCoordinatorApplyLanguage(f.documents, &current, "python3", &current) ==
              UMI_STATUS_OK &&
          current.revision == revision);
  } else if (!strcmp(name, "automatic-noop")) {
    CHECK(UmiDocumentCoordinatorApplyLanguage(f.documents, &before, NULL, &current) ==
              UMI_STATUS_OK &&
          current.revision == before.revision);
  } else if (!strcmp(name, "aba")) {
    CHECK(UmiDocumentCoordinatorApplyLanguage(f.documents, &before, "sql", &current) ==
          UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorApplyLanguage(f.documents, &current, NULL, &current) ==
          UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorApplyLanguage(f.documents, &before, NULL, NULL) ==
          UMI_STATUS_INVALID_STATE);
  } else if (!strcmp(name, "foreign-owner")) {
    ReplacementFixture other = {0};
    CHECK(Start(&other, "other") == 0);
    CHECK(UmiDocumentCoordinatorApplyLanguage(other.documents, &before, "sql", &current) ==
          UMI_STATUS_INVALID_STATE);
    Stop(&other);
  } else if (!strcmp(name, "foreign-view")) {
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
    UmiUiDocumentViewSnapshot v;
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &v) == UMI_STATUS_OK);
    strcpy(v.document_id, "foreign");
    CHECK(umi_ui_document_view_model_upsert(views, &v) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorInspectLanguage(f.documents, f.id, &current) ==
          UMI_STATUS_INVALID_STATE);
  } else if (!strcmp(name, "closed")) {
    CHECK(umi_document_coordinator_close_active(f.documents, 1) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorApplyLanguage(f.documents, &before, "sql", &current) ==
          UMI_STATUS_NOT_FOUND);
  } else if (!strcmp(name, "invalid-choice")) {
    CHECK(UmiDocumentCoordinatorApplyLanguage(f.documents, &before, "invalid", &current) ==
          UMI_STATUS_NOT_FOUND);
    CHECK(!memcmp(&current, &guard, sizeof guard));
    CHECK(UmiDocumentCoordinatorCheckLanguage(f.documents, &before) == UMI_STATUS_OK);
  } else if (!strcmp(name, "null-owner"))
    CHECK(UmiDocumentCoordinatorInspectLanguage(NULL, f.id, &current) ==
          UMI_STATUS_INVALID_ARGUMENT);
  else if (!strcmp(name, "null-review"))
    CHECK(UmiDocumentCoordinatorApplyLanguage(f.documents, NULL, "c", &current) ==
          UMI_STATUS_INVALID_ARGUMENT);
  else if (!strcmp(name, "null-output"))
    CHECK(UmiDocumentCoordinatorInspectLanguage(f.documents, f.id, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
  else if (!strcmp(name, "zero-id"))
    CHECK(UmiDocumentCoordinatorInspectLanguage(f.documents, 0U, &current) ==
          UMI_STATUS_INVALID_ARGUMENT);
  else if (!strcmp(name, "inactive")) {
    char other[UMI_UI_ID_CAPACITY];
    CHECK(umi_document_coordinator_new(f.documents, "other.py", other, sizeof other) ==
          UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorApplyLanguage(f.documents, &before, "sql", &current) ==
          UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK);
    CHECK(active.document_id != f.id && !strcmp(active.language_id, "python3"));
  } else {
    current = before;
    if (!strcmp(name, "tampered-id"))
      current.document_id = UINT64_MAX;
    if (!strcmp(name, "tampered-name"))
      strcpy(current.filename, "else.c");
    if (!strcmp(name, "tampered-effective"))
      strcpy(current.effective.language_id, "sql");
    if (!strcmp(name, "tampered-detected"))
      strcpy(current.detected.mime_type, "other");
    if (!strcmp(name, "tampered-override"))
      current.overridden = 1;
    if (!strcmp(name, "unterminated"))
      memset(current.filename, 'x', sizeof current.filename);
    CHECK(UmiDocumentCoordinatorApplyLanguage(f.documents, &current, "python3", NULL) ==
          (!strcmp(name, "tampered-id") ? UMI_STATUS_NOT_FOUND : UMI_STATUS_INVALID_STATE));
  }
  Stop(&f);
  return 0;
}
