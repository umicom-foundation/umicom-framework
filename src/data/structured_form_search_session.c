/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/structured_form_search_session.c
 *
 * PURPOSE:
 *   Compose saved-query validation with existing draft and immutable-save ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Compose saved-query validation with existing draft and immutable-save ownership. */
#include "umicom/data/structured_form_search_session.h"
#include "search_preset_internal.h"
#include <stdlib.h>
#include <string.h>
struct UmiStructuredFormSearchSession {
  const UmiStructuredFormSchema *schema;
  UmiRecordDraft *draft;
  char entity_prefix[64];
  uint64_t generation, edit_revision;
};
static UmiStatus state(const UmiStructuredFormSearchSession *session, UmiRecordDraftState *out) {
  return session ? umi_record_draft_state(session->draft, out) : UMI_STATUS_INVALID_ARGUMENT;
}
static UmiStatus replacement(const UmiStructuredFormSearchSession *session) {
  UmiRecordDraftState current;
  UmiStatus status = state(session, &current);
  if (status != UMI_STATUS_OK)
    return status;
  if (current.saving)
    return UMI_STATUS_BUSY;
  if (current.dirty)
    return UMI_STATUS_INVALID_STATE;
  return session->generation == UINT64_MAX ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
static UmiStatus context(const UmiStructuredFormSearchSession *session, uint64_t generation,
                         uint64_t edit_revision) {
  UmiRecordDraftState current;
  UmiStatus status = state(session, &current);
  if (status != UMI_STATUS_OK)
    return status;
  return current.ready && generation == session->generation &&
                 edit_revision == session->edit_revision
             ? UMI_STATUS_OK
             : UMI_STATUS_INVALID_STATE;
}
static UmiStatus definition(const UmiStructuredFormSearchSession *session,
                            const UmiStructuredFormSearchPreset *preset) {
  UmiStatus status = umi_structured_form_search_preset_validate(session->schema, preset);
  if (status == UMI_STATUS_OK && strcmp(session->entity_prefix, preset->entity_prefix))
    status = UMI_STATUS_PERMISSION_DENIED;
  return status;
}
static UmiStatus decode(const UmiStructuredFormSearchSession *session, const void *bytes,
                        size_t length, UmiStructuredFormSearchPreset *out) {
  UmiStatus status = umi_structured_form_search_preset_decode(session->schema, bytes, length, out);
  if (status == UMI_STATUS_OK)
    status = definition(session, out);
  return status;
}
UmiStatus umi_structured_form_search_session_create(const UmiStructuredFormSchema *schema,
                                                    const char *entity_prefix,
                                                    UmiStructuredFormSearchSession **out) {
  if (!out || !entity_prefix)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t length = 0;
  while (length < 64 && entity_prefix[length])
    ++length;
  if (!length || length >= 64)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStructuredFormSearchPreset *scope = calloc(1, sizeof *scope);
  UmiStructuredFormSearchSession *candidate = calloc(1, sizeof *candidate);
  if (!scope || !candidate) {
    free(scope);
    free(candidate);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  strcpy(scope->title, "Scope");
  memcpy(scope->entity_prefix, entity_prefix, length + 1);
  UmiStatus status = umi_structured_form_search_preset_validate(schema, scope);
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_create(&candidate->draft);
  if (status == UMI_STATUS_OK) {
    candidate->schema = schema;
    memcpy(candidate->entity_prefix, entity_prefix, length + 1);
    *out = candidate;
  } else
    free(candidate);
  free(scope);
  return status;
}
void umi_structured_form_search_session_destroy(UmiStructuredFormSearchSession *session) {
  if (session) {
    umi_record_draft_destroy(session->draft);
    free(session);
  }
}
UmiStatus umi_structured_form_search_session_begin(UmiStructuredFormSearchSession *session,
                                                   const char *entity_id,
                                                   const UmiStructuredFormSearchPreset *preset) {
  UmiStatus status = replacement(session);
  if (status == UMI_STATUS_OK)
    status = umi_search_preset_entity_check(entity_id);
  if (status == UMI_STATUS_OK)
    status = definition(session, preset);
  if (status != UMI_STATUS_OK)
    return status;
  UmiRecordDraftSnapshot *candidate = calloc(1, sizeof *candidate);
  if (!candidate)
    return UMI_STATUS_OUT_OF_MEMORY;
  strcpy(candidate->baseline.record.amendment.entity_id, entity_id);
  status = umi_structured_form_search_preset_encode(session->schema, preset, candidate->content,
                                                    sizeof candidate->content, &candidate->length);
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_snapshot_restore(session->draft, candidate);
  if (status == UMI_STATUS_OK) {
    ++session->generation;
    session->edit_revision = 1;
  }
  free(candidate);
  return status;
}
UmiStatus umi_structured_form_search_session_open(UmiStructuredFormSearchSession *session,
                                                  const UmiRecordDocument *document) {
  UmiStatus status = replacement(session);
  if (status != UMI_STATUS_OK)
    return status;
  if (!document)
    return UMI_STATUS_INVALID_ARGUMENT;
  status = umi_search_preset_entity_check(document->record.amendment.entity_id);
  if (status != UMI_STATUS_OK)
    return status;
  if (document->length > UMI_RECORD_DOCUMENT_MAX_BYTES)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiStructuredFormSearchPreset *preset = malloc(sizeof *preset);
  if (!preset)
    return UMI_STATUS_OUT_OF_MEMORY;
  status = decode(session, document->content, document->length, preset);
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_open(session->draft, document);
  if (status == UMI_STATUS_OK) {
    ++session->generation;
    session->edit_revision = 1;
  }
  free(preset);
  return status;
}
UmiStatus umi_structured_form_search_session_snapshot(const UmiStructuredFormSearchSession *session,
                                                      UmiStructuredFormSearchSessionView *out) {
  if (!session || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStructuredFormSearchSessionView *candidate = calloc(1, sizeof *candidate);
  unsigned char *bytes = malloc(UMI_FORM_SEARCH_PRESET_MAX_BYTES);
  if (!candidate || !bytes) {
    free(candidate);
    free(bytes);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  UmiStatus status = state(session, &candidate->record);
  size_t length = 0;
  if (status == UMI_STATUS_OK && candidate->record.ready) {
    status = umi_record_draft_copy_content(session->draft, bytes, UMI_FORM_SEARCH_PRESET_MAX_BYTES,
                                           &length);
    if (status == UMI_STATUS_OK)
      status = decode(session, bytes, length, &candidate->preset);
  }
  if (status == UMI_STATUS_OK) {
    candidate->generation = session->generation;
    candidate->edit_revision = session->edit_revision;
    *out = *candidate;
  }
  free(bytes);
  free(candidate);
  return status;
}
UmiStatus umi_structured_form_search_session_edit(UmiStructuredFormSearchSession *session,
                                                  uint64_t generation, uint64_t edit_revision,
                                                  const UmiStructuredFormSearchPreset *preset) {
  UmiStatus status = context(session, generation, edit_revision);
  if (status == UMI_STATUS_OK && session->edit_revision == UINT64_MAX)
    status = UMI_STATUS_CAPACITY_EXCEEDED;
  if (status == UMI_STATUS_OK)
    status = definition(session, preset);
  if (status != UMI_STATUS_OK)
    return status;
  unsigned char *bytes = malloc(UMI_FORM_SEARCH_PRESET_MAX_BYTES);
  if (!bytes)
    return UMI_STATUS_OUT_OF_MEMORY;
  size_t length = 0;
  status = umi_structured_form_search_preset_encode(session->schema, preset, bytes,
                                                    UMI_FORM_SEARCH_PRESET_MAX_BYTES, &length);
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_set_content(session->draft, bytes, length);
  if (status == UMI_STATUS_OK)
    ++session->edit_revision;
  free(bytes);
  return status;
}
UmiStatus umi_structured_form_search_session_discard(UmiStructuredFormSearchSession *session,
                                                     uint64_t generation, uint64_t edit_revision) {
  UmiStatus status = context(session, generation, edit_revision);
  if (status == UMI_STATUS_OK && session->generation == UINT64_MAX)
    status = UMI_STATUS_CAPACITY_EXCEEDED;
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_discard(session->draft);
  if (status == UMI_STATUS_OK) {
    UmiRecordDraftState current;
    status = state(session, &current);
    if (status == UMI_STATUS_OK) {
      ++session->generation;
      session->edit_revision = current.ready ? 1 : 0;
    }
  }
  return status;
}
UmiStatus umi_structured_form_search_session_prepare_save(
    UmiStructuredFormSearchSession *session, uint64_t generation, uint64_t edit_revision,
    const UmiRecordDraftAttribution *attribution, UmiRecordSaveRequest *out) {
  UmiStatus status = context(session, generation, edit_revision);
  return status == UMI_STATUS_OK ? umi_record_draft_prepare_save(session->draft, attribution, out)
                                 : status;
}
UmiStatus umi_structured_form_search_session_finish_save(UmiStructuredFormSearchSession *session,
                                                         uint64_t request_id, UmiStatus result,
                                                         const UmiRecordRevision *saved) {
  return session ? umi_record_draft_finish_save(session->draft, request_id, result, saved)
                 : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus umi_structured_form_search_session_capture(const UmiStructuredFormSearchSession *session,
                                                     UmiRecordDraftSnapshot *out) {
  return session ? umi_record_draft_snapshot_capture(session->draft, out)
                 : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus umi_structured_form_search_session_restore(UmiStructuredFormSearchSession *session,
                                                     const UmiRecordDraftSnapshot *snapshot) {
  UmiStatus status = replacement(session);
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_snapshot_validate(snapshot);
  if (status == UMI_STATUS_OK)
    status = umi_search_preset_entity_check(snapshot->baseline.record.amendment.entity_id);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredFormSearchPreset *preset = malloc(sizeof *preset);
  if (!preset)
    return UMI_STATUS_OUT_OF_MEMORY;
  status = decode(session, snapshot->content, snapshot->length, preset);
  if (status == UMI_STATUS_OK && snapshot->baseline.record.revision)
    status = decode(session, snapshot->baseline.content, snapshot->baseline.length, preset);
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_snapshot_restore(session->draft, snapshot);
  if (status == UMI_STATUS_OK) {
    ++session->generation;
    session->edit_revision = 1;
  }
  free(preset);
  return status;
}
