/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/form_records/main.c
 *
 * PURPOSE:
 *   Exercise the installed portable contract with a fictional record. No files,
 *   external services, native windows or source-tree fallback are required.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Exercise the installed portable contract with a fictional record. No files,
 * external services, native windows or source-tree fallback are required. */
#include "schema.h"
#include "umicom/data/record_catalogue.h"
#include "umicom/data/record_history_page.h"
#include "umicom/data/structured_form_conflict.h"
#include "umicom/data/structured_form_record.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(x)                                                                                 \
  do {                                                                                             \
    UmiStatus status = (x);                                                                        \
    if (status != UMI_STATUS_OK) {                                                                 \
      fprintf(stderr, "%s: status %d\n", #x, (int)status);                                         \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define EXPECT(x)                                                                                  \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "Unexpected result: %s\n", #x);                                              \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
typedef struct Example {
  UmiStructuredForm baseline, edited, incoming;
  UmiStructuredFormSessionView view;
  UmiStructuredFormEdit edit;
  UmiRecordDocument document;
  UmiRecordDraftSnapshot snapshot;
  UmiRecordSaveRequest request;
  UmiRecordCataloguePage catalogue;
  UmiRecordHistoryPage history;
  UmiStructuredFormRecord historical;
} Example;
static UmiStatus text(UmiStructuredForm *form, const char *id, const char *value) {
  UmiFormAnswer answer = {0};
  answer.state = UMI_FORM_KNOWN;
  UmiStatus status = umi_application_experience_value_text(&answer.value, value);
  return status == UMI_STATUS_OK
             ? umi_structured_form_set(&equipment_schema, form, form->revision, id, &answer)
             : status;
}
int main(void) {
  int result = 1;
  Example *e = calloc(1, sizeof(*e));
  UmiDataServer *store = NULL;
  UmiStructuredFormSession *session = NULL;
  UmiStructuredFormConflict *review = NULL;
  if (e == NULL) {
    fprintf(stderr, "Allocation failed\n");
    return 1;
  }
  REQUIRE(umi_data_server_create_memory(&store));
  REQUIRE(umi_structured_form_init(&equipment_schema, &e->baseline));
  REQUIRE(text(&e->baseline, "asset.label", "Fictional sensor"));
  REQUIRE(text(&e->baseline, "asset.location", "Shelf A"));
  UmiRecordAmendment metadata = {.entity_id = "example.asset.sensor",
                                 .source = "Equipment example",
                                 .actor = "Example author",
                                 .reason = "Initial location",
                                 .recorded_at_nanoseconds = 1U};
  UmiRecordRevision saved;
  REQUIRE(umi_structured_form_record_save(store, &metadata, &equipment_schema, &e->baseline, 0,
                                          &saved));
  REQUIRE(umi_record_document_latest(store, metadata.entity_id, &e->document));
  REQUIRE(umi_structured_form_session_create(&equipment_schema, &session));
  REQUIRE(umi_structured_form_session_open(session, &e->document));
  REQUIRE(umi_structured_form_session_snapshot(session, &e->view));
  e->edited = e->view.form;
  REQUIRE(text(&e->edited, "asset.location", "Shelf B"));
  REQUIRE(umi_structured_form_edit_capture(&equipment_schema, &e->view.form, &e->edited, &e->edit));
  REQUIRE(umi_structured_form_session_apply(session, e->view.generation, &e->edit));
  e->incoming = e->baseline;
  REQUIRE(text(&e->incoming, "asset.location", "Bench C"));
  REQUIRE(umi_structured_form_record_save(store, &metadata, &equipment_schema, &e->incoming, 1,
                                          &saved));
  REQUIRE(umi_structured_form_session_capture(session, &e->snapshot));
  REQUIRE(umi_record_draft_snapshot_check_store(store, &e->snapshot));
  REQUIRE(umi_record_document_latest(store, metadata.entity_id, &e->document));
  REQUIRE(umi_structured_form_session_review(session, e->view.generation, &e->document, &review));
  UmiFormMergeState review_state;
  REQUIRE(umi_structured_form_conflict_state(review, &review_state));
  EXPECT(review_state.unresolved_count == 1);
  REQUIRE(umi_structured_form_conflict_select(review, review_state.revision, "asset.location",
                                              UMI_FORM_MERGE_KEEP_LOCAL));
  REQUIRE(umi_structured_form_conflict_state(review, &review_state));
  REQUIRE(umi_structured_form_session_accept_review(session, e->view.generation, review,
                                                    review_state.revision, 1));
  REQUIRE(umi_structured_form_session_snapshot(session, &e->view));
  EXPECT(e->view.record.expected_revision == 2 && e->view.record.dirty);
  UmiRecordDraftAttribution attribution = {.source = "Equipment example",
                                           .actor = "Example reviewer",
                                           .reason = "Reviewed both location changes; kept Shelf B",
                                           .recorded_at_nanoseconds = 1U};
  REQUIRE(umi_structured_form_session_prepare_save(session, e->view.generation, &attribution, 1,
                                                   &e->request));
  REQUIRE(umi_record_save_request_commit(store, &e->request, &saved));
  REQUIRE(umi_structured_form_session_finish_save(session, e->request.request_id, UMI_STATUS_OK,
                                                  &saved));
  EXPECT(saved.revision == 3);
  UmiRecordCatalogueQuery query = {.entity_prefix = "example.asset.", .limit = 16};
  REQUIRE(umi_record_catalogue_page(store, &query, &e->catalogue));
  EXPECT(e->catalogue.count == 1 && e->catalogue.rows[0].revision == 3);
  REQUIRE(umi_record_history_page(store, metadata.entity_id, 0, 0, 16, &e->history));
  EXPECT(e->history.count == 3 && e->history.rows[2].revision == 1);
  REQUIRE(umi_structured_form_record_load(store, metadata.entity_id, 1, &equipment_schema,
                                          &e->historical));
  EXPECT(strcmp(e->historical.form.answers[1].value.text_value, "Shelf A") == 0);
  puts("Saved reviewed revision 3 at Shelf B; historical revision 1 remains Shelf A.");
  result = 0;
cleanup:
  umi_structured_form_conflict_destroy(review);
  umi_structured_form_session_destroy(session);
  umi_data_server_destroy(store);
  free(e);
  return result;
}
