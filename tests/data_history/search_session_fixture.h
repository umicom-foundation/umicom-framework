/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/search_session_fixture.h
 *
 * PURPOSE:
 *   One fictional setup for query-draft lifecycle, save and recovery regressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * One fictional setup for query-draft lifecycle, save and recovery regressions. */
#ifndef UMICOM_SEARCH_SESSION_FIXTURE_H
#define UMICOM_SEARCH_SESSION_FIXTURE_H
#include "../application_experience/structured_form_fixture.h"
#include "umicom/data/structured_form_search_session.h"
typedef struct SearchFixture {
  FormFixture *form;
  UmiDataServer *server;
  UmiStructuredFormSearchSession *session;
  UmiStructuredFormSearchPreset *preset;
  UmiStructuredFormSearchSessionView *view, *before;
  UmiRecordDraftSnapshot *snapshot;
  UmiRecordSaveRequest *request;
  UmiRecordDocument *document;
  UmiRecordDraftAttribution attribution;
  UmiRecordRevision saved;
} SearchFixture;
static inline void search_fixture_free(SearchFixture *f) {
  if (!f)
    return;
  umi_structured_form_search_session_destroy(f->session);
  umi_data_server_destroy(f->server);
  free(f->form);
  free(f->preset);
  free(f->view);
  free(f->before);
  free(f->snapshot);
  free(f->request);
  free(f->document);
  free(f);
}
/* mode zero creates an empty session; one begins a new query; two opens a
 * stored definition. SQLite uses a private in-memory database for these tests. */
static inline UmiStatus search_fixture_new(int mode, bool sqlite, SearchFixture **out) {
  SearchFixture *f = calloc(1, sizeof *f);
  if (!f)
    return UMI_STATUS_OUT_OF_MEMORY;
  f->form = form_fixture_new();
  f->preset = calloc(1, sizeof *f->preset);
  f->view = calloc(1, sizeof *f->view);
  f->before = malloc(sizeof *f->before);
  f->snapshot = calloc(1, sizeof *f->snapshot);
  f->request = calloc(1, sizeof *f->request);
  f->document = calloc(1, sizeof *f->document);
  UmiStatus status =
      f->form && f->preset && f->view && f->before && f->snapshot && f->request && f->document
          ? UMI_STATUS_OK
          : UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK)
    status = sqlite ? umi_data_server_create_sqlite(":memory:", &f->server)
                    : umi_data_server_create_memory(&f->server);
  if (status == UMI_STATUS_OK)
    status =
        umi_structured_form_search_session_create(&f->form->schema, "parcel.record.", &f->session);
  if (status == UMI_STATUS_OK) {
    strcpy(f->preset->title, "Baseline search");
    strcpy(f->preset->entity_prefix, "parcel.record.");
    f->preset->filter.count = 1;
    strcpy(f->preset->filter.terms[0].field_id, "parcel.mass");
    f->preset->filter.terms[0].operation = UMI_FORM_FILTER_LESS;
    f->preset->filter.terms[0].operand = form_mass(10);
    f->attribution = (UmiRecordDraftAttribution){.source = "Fictional draft",
                                                 .actor = "Example author",
                                                 .reason = "Save definition",
                                                 .recorded_at_nanoseconds = 2};
    if (mode == 1)
      status = umi_structured_form_search_session_begin(f->session, "form.search.new", f->preset);
    if (mode == 2) {
      UmiRecordAmendment metadata = {.entity_id = "form.search.example",
                                     .source = "Fictional definitions",
                                     .actor = "Example author",
                                     .reason = "Create query",
                                     .recorded_at_nanoseconds = 1};
      status = umi_structured_form_search_preset_save(f->server, &metadata, &f->form->schema,
                                                      f->preset, 0, &f->saved);
      if (status == UMI_STATUS_OK)
        status = umi_record_document_latest(f->server, metadata.entity_id, f->document);
      if (status == UMI_STATUS_OK)
        status = umi_structured_form_search_session_open(f->session, f->document);
    }
  }
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_snapshot(f->session, f->view);
  if (status == UMI_STATUS_OK) {
    *f->before = *f->view;
    *out = f;
  } else
    search_fixture_free(f);
  return status;
}
static inline UmiStatus search_fixture_edit(SearchFixture *f, const char *title) {
  strcpy(f->preset->title, title);
  UmiStatus status = umi_structured_form_search_session_edit(f->session, f->view->generation,
                                                             f->view->edit_revision, f->preset);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_snapshot(f->session, f->view);
  return status;
}
#endif
