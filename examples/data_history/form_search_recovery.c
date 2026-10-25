/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/data_history/form_search_recovery.c
 *
 * PURPOSE:
 *   Recover a query draft through the portable SDK, then save its captured request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Recover a query draft through the portable SDK, then save its captured request. */
#include "umicom/data/structured_form_search_session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const UmiApplicationExperienceField fields[] = {
    {.field_id = "quantity",
     .label = "Quantity",
     .help_text = "Fictional count",
     .kind = UMI_APPLICATION_EXPERIENCE_FIELD_INTEGER,
     .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER}};
static const UmiStructuredFormField rules[] = {
    {"quantity", "count.items", UMI_FORM_TEXT_PLAIN, 0, 0}};
static const UmiApplicationExperienceFormSchema base = {sizeof base,
                                                        UMI_APPLICATION_EXPERIENCE_API_VERSION,
                                                        "example.inventory",
                                                        "Inventory",
                                                        "Fictional inventory",
                                                        "example.save",
                                                        fields,
                                                        1};
static const UmiStructuredFormSchema schema = {&base, 1, rules, 1};
int main(void) {
  UmiStructuredFormSearchSession *editing = NULL, *restored = NULL;
  UmiDataServer *server = NULL;
  UmiStructuredFormSearchPreset *preset = calloc(1, sizeof *preset);
  UmiStructuredFormSearchSessionView *view = calloc(1, sizeof *view);
  UmiRecordDraftSnapshot *snapshot = calloc(1, sizeof *snapshot);
  UmiRecordSaveRequest *request = calloc(1, sizeof *request);
  unsigned char *bytes = NULL;
  size_t length = 0;
  UmiStatus status =
      preset && view && snapshot && request ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_create(&schema, "inventory.record.", &editing);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_create(&schema, "inventory.record.", &restored);
  if (status == UMI_STATUS_OK) {
    strcpy(preset->title, "Inventory overview");
    strcpy(preset->entity_prefix, "inventory.record.");
    status = umi_structured_form_search_session_begin(editing, "form.search.inventory", preset);
  }
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_capture(editing, snapshot);
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_snapshot_encode(snapshot, &bytes, &length);
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_snapshot_decode(bytes, length, snapshot);
  if (status == UMI_STATUS_OK)
    status = umi_data_server_create_memory(&server);
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_snapshot_check_store(server, snapshot);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_restore(restored, snapshot);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_snapshot(restored, view);
  UmiRecordDraftAttribution attribution = {.source = "SDK example",
                                           .actor = "Example author",
                                           .reason = "Save recovered definition",
                                           .recorded_at_nanoseconds = 1};
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_prepare_save(
        restored, view->generation, view->edit_revision, &attribution, request);
  if (status == UMI_STATUS_OK) {
    UmiRecordRevision saved;
    UmiStatus committed = umi_record_save_request_commit(server, request, &saved);
    status = umi_structured_form_search_session_finish_save(
        restored, request->request_id, committed, committed == UMI_STATUS_OK ? &saved : NULL);
  }
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_snapshot(restored, view);
  if (status == UMI_STATUS_OK &&
      (view->record.dirty || view->record.saving || view->record.expected_revision != 1))
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    printf("Recovered and saved: %s\n", view->preset.title);
  else
    fprintf(stderr, "Recovery failed: %d\n", (int)status);
  umi_record_draft_snapshot_bytes_free(bytes);
  umi_structured_form_search_session_destroy(editing);
  umi_structured_form_search_session_destroy(restored);
  umi_data_server_destroy(server);
  free(preset);
  free(view);
  free(snapshot);
  free(request);
  return status == UMI_STATUS_OK ? 0 : 1;
}
