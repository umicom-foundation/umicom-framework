/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Draft ownership binds commands to one definition and one observed edit. */
#include "search_session_fixture.h"
#undef CHECK
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
  const char *cases[] = {"empty",          "begin",         "open",           "edit",
                         "no-op",          "revert",        "stale-edit",     "stale-generation",
                         "dirty-begin",    "dirty-open",    "discard-new",    "discard-saved",
                         "reopen",         "invalid-title", "invalid-filter", "wrong-scope",
                         "bad-entity",     "open-digest",   "open-scope",     "open-schema",
                         "create-invalid", "null"};
  if (argc != 2)
    return 2;
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(argv[1], cases[i]))
      known = true;
  if (!known)
    return 2;
  int result = 0;
  const char *name = argv[1];
  SearchFixture *f = NULL;
  UmiStructuredFormSearchSession *second = NULL;
  int mode = !strcmp(name, "empty") || !strcmp(name, "begin") || !strcmp(name, "bad-entity") ? 0
             : !strcmp(name, "discard-new")                                                  ? 1
                                                                                             : 2;
  OK(search_fixture_new(mode, false, &f));
  if (!strcmp(name, "empty")) {
    CHECK(!f->view->record.ready && !f->view->generation && !f->view->edit_revision);
    goto cleanup;
  }
  if (!strcmp(name, "create-invalid")) {
    CHECK(umi_structured_form_search_session_create(NULL, "parcel.record.", &second) !=
              UMI_STATUS_OK &&
          !second);
    CHECK(umi_structured_form_search_session_create(&f->form->schema, "bad/", &second) !=
              UMI_STATUS_OK &&
          !second);
    CHECK(umi_structured_form_search_session_create(&f->form->schema, NULL, &second) !=
              UMI_STATUS_OK &&
          !second);
    goto cleanup;
  }
  if (!strcmp(name, "null")) {
    CHECK(umi_structured_form_search_session_snapshot(NULL, f->view) != UMI_STATUS_OK);
    CHECK(umi_structured_form_search_session_snapshot(f->session, NULL) != UMI_STATUS_OK);
    CHECK(umi_structured_form_search_session_edit(f->session, f->view->generation,
                                                  f->view->edit_revision, NULL) != UMI_STATUS_OK);
    CHECK(umi_structured_form_search_session_open(f->session, NULL) != UMI_STATUS_OK);
    CHECK(umi_structured_form_search_session_discard(NULL, 1, 1) != UMI_STATUS_OK);
    goto cleanup;
  }
  if (!strcmp(name, "begin") || !strcmp(name, "bad-entity")) {
    UmiStatus status = umi_structured_form_search_session_begin(
        f->session, !strcmp(name, "bad-entity") ? "other.query" : "form.search.new", f->preset);
    if (!strcmp(name, "bad-entity")) {
      CHECK(status != UMI_STATUS_OK);
      OK(umi_structured_form_search_session_snapshot(f->session, f->view));
      CHECK(!memcmp(f->view, f->before, sizeof *f->view));
    } else {
      OK(status);
      OK(umi_structured_form_search_session_snapshot(f->session, f->view));
      CHECK(f->view->record.dirty && f->view->record.is_new && f->view->generation == 1 &&
            f->view->edit_revision == 1);
    }
    goto cleanup;
  }
  CHECK(f->view->generation == 1 && f->view->edit_revision == 1);
  if (!strcmp(name, "open")) {
    CHECK(!f->view->record.dirty && f->view->record.expected_revision == 1 &&
          !strcmp(f->view->preset.title, "Baseline search"));
    goto cleanup;
  }
  if (!strncmp(name, "open-", 5)) {
    if (!strcmp(name, "open-digest"))
      f->document->content[0] ^= 1;
    if (!strcmp(name, "open-schema"))
      f->form->schema.version = 2;
    if (!strcmp(name, "open-scope")) {
      OK(umi_structured_form_search_session_create(&f->form->schema, "other.record.", &second));
      CHECK(umi_structured_form_search_session_open(second, f->document) ==
            UMI_STATUS_PERMISSION_DENIED);
    } else
      CHECK(umi_structured_form_search_session_open(f->session, f->document) != UMI_STATUS_OK);
    f->form->schema.version = 1;
    OK(umi_structured_form_search_session_snapshot(f->session, f->view));
    CHECK(!memcmp(f->view, f->before, sizeof *f->view));
    goto cleanup;
  }
  if (!strcmp(name, "reopen")) {
    OK(umi_structured_form_search_session_open(f->session, f->document));
    OK(umi_structured_form_search_session_snapshot(f->session, f->view));
    CHECK(f->view->generation == 2 && f->view->edit_revision == 1);
    CHECK(umi_structured_form_search_session_edit(f->session, 1, 1, f->preset) ==
          UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (!strcmp(name, "invalid-title") || !strcmp(name, "invalid-filter") ||
      !strcmp(name, "wrong-scope")) {
    if (!strcmp(name, "invalid-title"))
      f->preset->title[0] = 0;
    if (!strcmp(name, "invalid-filter"))
      f->preset->filter.count = 9;
    if (!strcmp(name, "wrong-scope"))
      strcpy(f->preset->entity_prefix, "other.record.");
    CHECK(umi_structured_form_search_session_edit(f->session, 1, 1, f->preset) != UMI_STATUS_OK);
    OK(umi_structured_form_search_session_snapshot(f->session, f->view));
    CHECK(!memcmp(f->view, f->before, sizeof *f->view));
    goto cleanup;
  }
  if (!strcmp(name, "no-op")) {
    OK(search_fixture_edit(f, "Baseline search"));
    CHECK(!f->view->record.dirty && f->view->edit_revision == 2);
    goto cleanup;
  }
  OK(search_fixture_edit(f, "Changed search"));
  CHECK(f->view->record.dirty && f->view->edit_revision == 2);
  *f->before = *f->view;
  if (!strcmp(name, "stale-edit") || !strcmp(name, "stale-generation")) {
    CHECK(umi_structured_form_search_session_edit(
              f->session, !strcmp(name, "stale-generation") ? 0 : 1,
              !strcmp(name, "stale-edit") ? 1 : 2, f->preset) == UMI_STATUS_INVALID_STATE);
    OK(umi_structured_form_search_session_snapshot(f->session, f->view));
    CHECK(!memcmp(f->view, f->before, sizeof *f->view));
  }
  if (!strcmp(name, "dirty-open"))
    CHECK(umi_structured_form_search_session_open(f->session, f->document) ==
          UMI_STATUS_INVALID_STATE);
  if (!strcmp(name, "dirty-begin"))
    CHECK(umi_structured_form_search_session_begin(f->session, "form.search.other", f->preset) ==
          UMI_STATUS_INVALID_STATE);
  if (!strcmp(name, "revert")) {
    OK(search_fixture_edit(f, "Baseline search"));
    CHECK(!f->view->record.dirty && f->view->edit_revision == 3);
  }
  if (!strcmp(name, "discard-new") || !strcmp(name, "discard-saved")) {
    OK(umi_structured_form_search_session_discard(f->session, f->view->generation,
                                                  f->view->edit_revision));
    OK(umi_structured_form_search_session_snapshot(f->session, f->view));
    CHECK(f->view->generation == 2 && !f->view->record.dirty);
    if (!strcmp(name, "discard-new"))
      CHECK(!f->view->record.ready && !f->view->edit_revision);
    else
      CHECK(f->view->record.ready && !strcmp(f->view->preset.title, "Baseline search"));
  }
cleanup:
  umi_structured_form_search_session_destroy(second);
  search_fixture_free(f);
  return result;
}
