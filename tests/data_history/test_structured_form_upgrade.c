/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_structured_form_upgrade.c
 *
 * PURPOSE:
 *   Upgrades retain the original history, require explicit review and bind worker
 *   requests to the same baseline in both supported local storage backends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Upgrades retain the original history, require explicit review and bind worker
 * requests to the same baseline in both supported local storage backends. */
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "umicom/data/structured_form_record.h"
#include "umicom/data/structured_form_upgrade.h"
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
int main(int argc, char **argv) {
  if (argc != 2 && argc != 3)
    return 2;
  const char *cases[] = {"compatible",    "add-default",  "removed-unanswered",
                         "removed-known", "unit",         "wrong-version",
                         "wrong-schema",  "corrupt",      "source-copy",
                         "edit",          "invalid-edit", "stale-review",
                         "required",      "pending",      "wrong-reply",
                         "failed-retry",  "wrong-store",  "changed-request",
                         "stale-store",   "history",      "noop",
                         "new-source"};
  bool selected = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      selected = true;
  if (!selected || (argc == 3 && strcmp(argv[2], "sqlite") != 0))
    return 2;
  int result = 0;
  const char *name = argv[1];
  FormFixture *f = form_fixture_new(), *target = form_fixture_new();
  UmiDataServer *store = NULL, *other = NULL;
  UmiStructuredFormUpgrade *upgrade = NULL;
  UmiRecordDocument *document = calloc(1, sizeof(*document));
  UmiStructuredFormUpgradeView *view = calloc(1, sizeof(*view));
  UmiStructuredFormUpgradeRequest *request = calloc(1, sizeof(*request)),
                                  *repeat = calloc(1, sizeof(*repeat));
  UmiStructuredFormEdit *edit = calloc(1, sizeof(*edit));
  UmiStructuredFormRecord *loaded = calloc(1, sizeof(*loaded));
  CHECK(f && target && document && view && request && repeat && edit && loaded);
  target->schema.version = 2;
  if (argc == 3) {
    OK(umi_data_server_create_sqlite(":memory:", &store));
    OK(umi_data_server_create_sqlite(":memory:", &other));
  } else {
    OK(umi_data_server_create_memory(&store));
    OK(umi_data_server_create_memory(&other));
  }
  UmiRecordAmendment metadata = {.entity_id = "upgrade.parcel",
                                 .source = "Fictional source",
                                 .actor = "Example author",
                                 .reason = "Initial form",
                                 .recorded_at_nanoseconds = 1U};
  UmiRecordDraftAttribution attribution = {.source = "Fictional source",
                                           .actor = "Example reviewer",
                                           .reason = "Reviewed schema upgrade",
                                           .recorded_at_nanoseconds = 1U};
  UmiRecordRevision saved;
  UmiFormAnswer answer = form_mass(9);
  OK(umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.mass", &answer));
  if (strcmp(name, "removed-known") == 0) {
    answer = form_text("Must be retained");
    OK(umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.note", &answer));
  }
  OK(umi_structured_form_record_save(store, &metadata, &f->schema, &f->before, 0, &saved));
  OK(umi_record_document_latest(store, "upgrade.parcel", document));
  if (strcmp(name, "add-default") == 0) {
    target->fields[2].field_id = "parcel.category";
    target->rules[2].field_id = "parcel.category";
    OK(umi_application_experience_value_text(&target->fields[2].default_value, "general"));
  }
  if (strcmp(name, "removed-unanswered") == 0 || strcmp(name, "removed-known") == 0)
    target->base.field_count = target->schema.rule_count = 2;
  if (strcmp(name, "unit") == 0)
    target->rules[0].unit_id = "mass.lb";
  if (strcmp(name, "wrong-version") == 0)
    target->schema.version = 1;
  if (strcmp(name, "wrong-schema") == 0)
    target->base.form_id = "example.different";
  if (strcmp(name, "corrupt") == 0)
    document->content[0] ^= 1;
  if (strcmp(name, "new-source") == 0)
    document->record.revision = 0;
  UmiStatus created =
      umi_structured_form_upgrade_create(&f->schema, document, &target->schema, &upgrade);
  if (strcmp(name, "removed-known") == 0 || strcmp(name, "unit") == 0 ||
      strcmp(name, "wrong-version") == 0 || strcmp(name, "wrong-schema") == 0 ||
      strcmp(name, "corrupt") == 0 || strcmp(name, "new-source") == 0) {
    CHECK(created != UMI_STATUS_OK && upgrade == NULL);
    goto cleanup;
  }
  OK(created);
  OK(umi_structured_form_upgrade_snapshot(upgrade, view));
  CHECK(view->revision == 1 && view->source_schema_version == 1 &&
        view->target_schema_version == 2 && view->record.expected_revision == 1 &&
        view->record.dirty && !view->committed);
  CHECK(view->proposed.answers[0].value.decimal_value == 9);
  if (strcmp(name, "add-default") == 0) {
    CHECK(view->retained_fields == 2 && view->added_fields == 1 &&
          view->removed_unanswered_fields == 1);
    CHECK(strcmp(view->proposed.answers[2].value.text_value, "general") == 0);
  } else if (strcmp(name, "removed-unanswered") == 0)
    CHECK(view->retained_fields == 2 && view->added_fields == 0 &&
          view->removed_unanswered_fields == 1);
  else
    CHECK(view->retained_fields == 3 && view->added_fields == 0 &&
          view->removed_unanswered_fields == 0);
  if (strcmp(name, "source-copy") == 0)
    memset(document->content, 0, document->length);
  f->after = view->proposed;
  if (strcmp(name, "noop") != 0) {
    answer = form_mass(11);
    OK(umi_structured_form_set(&target->schema, &f->after, f->after.revision, "parcel.mass",
                               &answer));
  }
  OK(umi_structured_form_edit_capture(&target->schema, &view->proposed, &f->after, edit));
  if (strcmp(name, "invalid-edit") == 0) {
    memcpy(edit->changes[0].after.unit_id, "mass.lb", 8);
    CHECK(umi_structured_form_upgrade_apply(upgrade, 1, edit) != UMI_STATUS_OK);
    OK(umi_structured_form_upgrade_snapshot(upgrade, view));
    CHECK(view->revision == 1 && view->proposed.answers[0].value.decimal_value == 9);
    goto cleanup;
  }
  if (strcmp(name, "edit") == 0 || strcmp(name, "noop") == 0) {
    OK(umi_structured_form_upgrade_apply(upgrade, 1, edit));
    OK(umi_structured_form_upgrade_snapshot(upgrade, view));
    CHECK(view->revision == (strcmp(name, "edit") == 0 ? 2U : 1U));
  }
  if (strcmp(name, "stale-review") == 0) {
    CHECK(umi_structured_form_upgrade_apply(upgrade, 2, edit) == UMI_STATUS_INVALID_STATE);
    CHECK(umi_structured_form_upgrade_prepare_save(upgrade, 2, &attribution, 0, request) ==
          UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "required") == 0) {
    CHECK(umi_structured_form_upgrade_prepare_save(upgrade, view->revision, &attribution, 1,
                                                   request) == UMI_STATUS_INVALID_STATE);
    OK(umi_structured_form_upgrade_snapshot(upgrade, view));
    CHECK(!view->record.saving);
    goto cleanup;
  }
  OK(umi_structured_form_upgrade_prepare_save(upgrade, view->revision, &attribution, 0, request));
  CHECK(request->save.proposed.previous_revision == 1 &&
        request->snapshot.baseline.record.revision == 1);
  if (strcmp(name, "pending") == 0) {
    CHECK(umi_structured_form_upgrade_apply(upgrade, view->revision, edit) == UMI_STATUS_BUSY);
    CHECK(umi_structured_form_upgrade_prepare_save(upgrade, view->revision, &attribution, 0,
                                                   repeat) == UMI_STATUS_BUSY);
    CHECK(umi_structured_form_upgrade_finish_save(upgrade, request->save.request_id,
                                                  UMI_STATUS_CANCELLED,
                                                  NULL) == UMI_STATUS_CANCELLED);
    goto cleanup;
  }
  if (strcmp(name, "failed-retry") == 0) {
    uint64_t first = request->save.request_id;
    CHECK(umi_structured_form_upgrade_finish_save(upgrade, first, UMI_STATUS_CANCELLED, NULL) ==
          UMI_STATUS_CANCELLED);
    OK(umi_structured_form_upgrade_prepare_save(upgrade, view->revision, &attribution, 0, request));
    CHECK(request->save.request_id > first);
  }
  if (strcmp(name, "wrong-store") == 0) {
    answer = form_mass(10);
    OK(umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.mass", &answer));
    OK(umi_structured_form_record_save(other, &metadata, &f->schema, &f->before, 0, &saved));
    CHECK(umi_structured_form_upgrade_request_commit(other, request, &saved) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(umi_structured_form_upgrade_finish_save(upgrade, request->save.request_id,
                                                  UMI_STATUS_INVALID_STATE,
                                                  NULL) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "changed-request") == 0) {
    request->save.content[0] ^= 1;
    CHECK(umi_structured_form_upgrade_request_commit(store, request, &saved) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(umi_structured_form_upgrade_finish_save(upgrade, request->save.request_id,
                                                  UMI_STATUS_INVALID_STATE,
                                                  NULL) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "stale-store") == 0) {
    answer = form_mass(12);
    OK(umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.mass", &answer));
    OK(umi_structured_form_record_save(store, &metadata, &f->schema, &f->before, 1, &saved));
    CHECK(umi_structured_form_upgrade_request_commit(store, request, &saved) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(umi_structured_form_upgrade_finish_save(upgrade, request->save.request_id,
                                                  UMI_STATUS_INVALID_STATE,
                                                  NULL) == UMI_STATUS_INVALID_STATE);
    OK(umi_structured_form_upgrade_snapshot(upgrade, view));
    CHECK(view->record.dirty && !view->committed);
    goto cleanup;
  }
  OK(umi_structured_form_upgrade_request_commit(store, request, &saved));
  CHECK(saved.revision == 2);
  if (strcmp(name, "wrong-reply") == 0) {
    CHECK(umi_structured_form_upgrade_finish_save(upgrade, request->save.request_id + 1,
                                                  UMI_STATUS_OK,
                                                  &saved) == UMI_STATUS_INVALID_STATE);
    OK(umi_structured_form_upgrade_snapshot(upgrade, view));
    CHECK(view->record.saving && !view->committed);
  }
  OK(umi_structured_form_upgrade_finish_save(upgrade, request->save.request_id, UMI_STATUS_OK,
                                             &saved));
  OK(umi_structured_form_upgrade_snapshot(upgrade, view));
  CHECK(view->committed && !view->record.dirty && !view->record.saving &&
        view->record.expected_revision == 2);
  CHECK(umi_structured_form_upgrade_prepare_save(upgrade, view->revision, &attribution, 0,
                                                 repeat) == UMI_STATUS_INVALID_STATE);
  CHECK(umi_structured_form_upgrade_apply(upgrade, view->revision, edit) ==
        UMI_STATUS_INVALID_STATE);
  OK(umi_structured_form_record_load(store, "upgrade.parcel", 1, &f->schema, loaded));
  CHECK(loaded->form.schema_version == 1 && loaded->form.answers[0].value.decimal_value == 9);
  OK(umi_structured_form_record_load(store, "upgrade.parcel", 2, &target->schema, loaded));
  CHECK(loaded->form.schema_version == 2 &&
        loaded->form.answers[0].value.decimal_value == (strcmp(name, "edit") == 0 ? 11 : 9));
  if (strcmp(name, "history") == 0) {
    CHECK(umi_structured_form_record_load(store, "upgrade.parcel", 2, &f->schema, loaded) ==
          UMI_STATUS_INVALID_STATE);
    OK(umi_record_document_latest(store, "upgrade.parcel", document));
    CHECK(document->record.revision == 2 && document->record.previous_revision == 1);
  }
cleanup:
  umi_structured_form_upgrade_destroy(upgrade);
  umi_data_server_destroy(other);
  umi_data_server_destroy(store);
  free(loaded);
  free(edit);
  free(repeat);
  free(request);
  free(view);
  free(document);
  free(target);
  free(f);
  return result;
}
