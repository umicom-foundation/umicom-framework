/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Schema migration and storage share one reviewed draft. Ordinary editing does
 * not silently reinterpret old records or bypass final storage concurrency. */
#include "umicom/data/structured_form_upgrade.h"
#include "umicom/application/experience/structured_form_migration.h"
#include "umicom/application/experience/structured_form_snapshot.h"
#include <stdlib.h>
#include <string.h>
struct UmiStructuredFormUpgrade {
  const UmiStructuredFormSchema *target;
  UmiRecordDraft *draft;
  UmiStructuredForm proposed;
  uint64_t revision, source_version;
  size_t retained, added, removed;
  bool committed;
};
static UmiStatus encode(const UmiStructuredFormSchema *schema, const UmiStructuredForm *form,
                        UmiRecordDraft *draft) {
  unsigned char *bytes = malloc(UMI_RECORD_DOCUMENT_MAX_BYTES);
  if (bytes == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  size_t length = 0;
  UmiStatus status = umi_structured_form_snapshot_encode(schema, form, bytes,
                                                         UMI_RECORD_DOCUMENT_MAX_BYTES, &length);
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_set_content(draft, bytes, length);
  free(bytes);
  return status;
}
UmiStatus umi_structured_form_upgrade_create(const UmiStructuredFormSchema *source_schema,
                                             const UmiRecordDocument *document,
                                             const UmiStructuredFormSchema *target_schema,
                                             UmiStructuredFormUpgrade **out) {
  if (document == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStructuredFormUpgrade *upgrade = calloc(1, sizeof(*upgrade));
  UmiStructuredForm *source = malloc(sizeof(*source));
  if (upgrade == NULL || source == NULL) {
    free(upgrade);
    free(source);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  UmiStatus status = umi_record_draft_create(&upgrade->draft);
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_open(upgrade->draft, document);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_snapshot_decode(source_schema, document->content, document->length,
                                                 source);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_migrate(source_schema, source, target_schema, &upgrade->proposed);
  if (status == UMI_STATUS_OK)
    status = encode(target_schema, &upgrade->proposed, upgrade->draft);
  if (status == UMI_STATUS_OK) {
    upgrade->target = target_schema;
    upgrade->source_version = source_schema->version;
    upgrade->revision = 1;
    for (size_t i = 0; i < source_schema->base->field_count; ++i) {
      if (umi_application_experience_form_schema_index(
              target_schema->base, source_schema->base->fields[i].field_id) == SIZE_MAX)
        ++upgrade->removed;
      else
        ++upgrade->retained;
    }
    upgrade->added = target_schema->base->field_count - upgrade->retained;
    *out = upgrade;
  } else
    umi_structured_form_upgrade_destroy(upgrade);
  free(source);
  return status;
}
void umi_structured_form_upgrade_destroy(UmiStructuredFormUpgrade *upgrade) {
  if (upgrade != NULL) {
    umi_record_draft_destroy(upgrade->draft);
    free(upgrade);
  }
}
UmiStatus umi_structured_form_upgrade_snapshot(const UmiStructuredFormUpgrade *upgrade,
                                               UmiStructuredFormUpgradeView *out) {
  if (upgrade == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiRecordDraftState state;
  UmiStatus status = umi_record_draft_state(upgrade->draft, &state);
  if (status != UMI_STATUS_OK)
    return status;
  out->revision = upgrade->revision;
  out->source_schema_version = upgrade->source_version;
  out->target_schema_version = upgrade->target->version;
  out->retained_fields = upgrade->retained;
  out->added_fields = upgrade->added;
  out->removed_unanswered_fields = upgrade->removed;
  out->committed = upgrade->committed;
  out->record = state;
  out->proposed = upgrade->proposed;
  return UMI_STATUS_OK;
}
static UmiStatus editable(UmiStructuredFormUpgrade *upgrade, uint64_t expected) {
  if (upgrade == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiRecordDraftState state;
  UmiStatus status = umi_record_draft_state(upgrade->draft, &state);
  if (status != UMI_STATUS_OK)
    return status;
  if (state.saving)
    return UMI_STATUS_BUSY;
  return upgrade->committed || upgrade->revision != expected ? UMI_STATUS_INVALID_STATE
                                                             : UMI_STATUS_OK;
}
UmiStatus umi_structured_form_upgrade_apply(UmiStructuredFormUpgrade *upgrade,
                                            uint64_t expected_revision,
                                            const UmiStructuredFormEdit *edit) {
  UmiStatus status = editable(upgrade, expected_revision);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredForm *candidate = malloc(sizeof(*candidate));
  if (candidate == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  *candidate = upgrade->proposed;
  status = umi_structured_form_edit_apply(upgrade->target, candidate, edit);
  if (status == UMI_STATUS_OK && candidate->revision != upgrade->proposed.revision) {
    if (upgrade->revision == UINT64_MAX)
      status = UMI_STATUS_CAPACITY_EXCEEDED;
    else
      status = encode(upgrade->target, candidate, upgrade->draft);
    if (status == UMI_STATUS_OK) {
      upgrade->proposed = *candidate;
      ++upgrade->revision;
    }
  }
  free(candidate);
  return status;
}
UmiStatus umi_structured_form_upgrade_prepare_save(UmiStructuredFormUpgrade *upgrade,
                                                   uint64_t expected_revision,
                                                   const UmiRecordDraftAttribution *attribution,
                                                   int submitting,
                                                   UmiStructuredFormUpgradeRequest *out) {
  if (out == NULL || attribution == NULL || (submitting != 0 && submitting != 1))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = editable(upgrade, expected_revision);
  if (status != UMI_STATUS_OK)
    return status;
  UmiApplicationExperienceValidationReport *report = malloc(sizeof(*report));
  UmiStructuredFormUpgradeRequest *request = calloc(1, sizeof(*request));
  if (report == NULL || request == NULL) {
    free(report);
    free(request);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  status = umi_structured_form_validate(upgrade->target, &upgrade->proposed, submitting, report);
  if (status == UMI_STATUS_OK && report->error_count != 0)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_snapshot_capture(upgrade->draft, &request->snapshot);
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_prepare_save(upgrade->draft, attribution, &request->save);
  if (status == UMI_STATUS_OK)
    *out = *request;
  free(report);
  free(request);
  return status;
}
UmiStatus umi_structured_form_upgrade_request_commit(UmiDataServer *server,
                                                     const UmiStructuredFormUpgradeRequest *request,
                                                     UmiRecordRevision *out) {
  if (server == NULL || request == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_record_draft_snapshot_validate(&request->snapshot);
  if (status != UMI_STATUS_OK)
    return status;
  if (request->snapshot.baseline.record.revision == 0 ||
      request->save.proposed.previous_revision != request->snapshot.baseline.record.revision ||
      request->save.length != request->snapshot.length ||
      memcmp(request->save.content, request->snapshot.content, request->snapshot.length) != 0 ||
      memchr(request->save.proposed.amendment.entity_id, '\0',
             sizeof(request->save.proposed.amendment.entity_id)) == NULL ||
      strcmp(request->save.proposed.amendment.entity_id,
             request->snapshot.baseline.record.amendment.entity_id) != 0)
    return UMI_STATUS_INVALID_STATE;
  status = umi_record_draft_snapshot_check_store(server, &request->snapshot);
  if (status == UMI_STATUS_OK)
    status = umi_record_save_request_commit(server, &request->save, out);
  return status;
}
UmiStatus umi_structured_form_upgrade_finish_save(UmiStructuredFormUpgrade *upgrade,
                                                  uint64_t request_id, UmiStatus result,
                                                  const UmiRecordRevision *saved) {
  if (upgrade == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_record_draft_finish_save(upgrade->draft, request_id, result, saved);
  if (status == UMI_STATUS_OK) {
    upgrade->committed = true;
    upgrade->proposed.saved_revision = upgrade->proposed.revision;
  }
  return status;
}
