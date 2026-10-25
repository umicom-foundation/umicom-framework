/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/language_override.inc
 *
 * PURPOSE:
 *   Keep checked language metadata with the existing coordinator and publish metadata without
 * replacing pending text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/language_override.h"
static int LanguageIdentitySame(const UmiDocumentLanguageIdentity *a,
                                const UmiDocumentLanguageIdentity *b) {
  return memchr(a->language_id, 0, sizeof a->language_id) &&
         memchr(b->language_id, 0, sizeof b->language_id) &&
         memchr(a->mime_type, 0, sizeof a->mime_type) &&
         memchr(b->mime_type, 0, sizeof b->mime_type) &&
         memchr(a->icon_name, 0, sizeof a->icon_name) &&
         memchr(b->icon_name, 0, sizeof b->icon_name) && !strcmp(a->language_id, b->language_id) &&
         !strcmp(a->mime_type, b->mime_type) && !strcmp(a->icon_name, b->icon_name) &&
         a->text == b->text;
}
UmiStatus UmiDocumentCoordinatorInspectLanguage(const UmiDocumentCoordinator *owner,
                                                UmiDocumentId id, UmiDocumentLanguageReview *out) {
  if (owner == NULL || id == 0U || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t index = EditDocumentIndex(owner, id);
  if (index == SIZE_MAX)
    return UMI_STATUS_NOT_FOUND;
  const UmiDocumentCoordinatorEntry *entry = &owner->entries[index];
  UmiDocumentSnapshot stored;
  UmiStatus status = umi_document_store_snapshot(owner->store, id, &stored);
  if (status != UMI_STATUS_OK)
    return status;
  UmiUiDocumentViewSnapshot view;
  status = umi_ui_document_view_model_find(umi_ui_workbench_documents(owner->workbench),
                                           entry->view_id, &view);
  if (status != UMI_STATUS_OK)
    return status;
  char identity[UMI_UI_ID_CAPACITY];
  (void)snprintf(identity, sizeof identity, "document.%llu", (unsigned long long)id);
  if (strcmp(identity, view.document_id))
    return UMI_STATUS_INVALID_STATE;
  UmiDocumentLanguageReview result = {0};
  result.owner = owner;
  result.document_id = id;
  result.revision = entry->language_revision;
  (void)snprintf(result.filename, sizeof result.filename, "%s", stored.display_name);
  status = umi_document_language_detect(stored.display_name, &result.detected);
  if (status != UMI_STATUS_OK)
    return status;
  result.overridden = entry->has_language_override;
  result.effective = result.overridden ? entry->language_override : result.detected;
  if (strcmp(result.effective.language_id, view.language_id))
    return UMI_STATUS_INVALID_STATE;
  *out = result;
  return UMI_STATUS_OK;
}
UmiStatus UmiDocumentCoordinatorCheckLanguage(const UmiDocumentCoordinator *owner,
                                              const UmiDocumentLanguageReview *review) {
  if (owner == NULL || review == NULL || review->document_id == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (review->owner != owner)
    return UMI_STATUS_INVALID_STATE;
  UmiDocumentLanguageReview current;
  UmiStatus status = UmiDocumentCoordinatorInspectLanguage(owner, review->document_id, &current);
  if (status != UMI_STATUS_OK)
    return status;
  if (review->revision != current.revision || review->overridden != current.overridden ||
      !memchr(review->filename, 0, sizeof review->filename) ||
      strcmp(review->filename, current.filename) ||
      !LanguageIdentitySame(&review->detected, &current.detected) ||
      !LanguageIdentitySame(&review->effective, &current.effective))
    return UMI_STATUS_INVALID_STATE;
  return UMI_STATUS_OK;
}
UmiStatus UmiDocumentCoordinatorApplyLanguage(UmiDocumentCoordinator *owner,
                                              const UmiDocumentLanguageReview *review,
                                              const char *id, UmiDocumentLanguageReview *out) {
  UmiStatus status = UmiDocumentCoordinatorCheckLanguage(owner, review);
  if (status != UMI_STATUS_OK)
    return status;
  UmiDocumentLanguageIdentity chosen = review->detected;
  if (id != NULL) {
    status = UmiDocumentLanguageChoiceIdentity(id, &chosen);
    if (status != UMI_STATUS_OK)
      return status;
  }
  size_t index = EditDocumentIndex(owner, review->document_id);
  UmiDocumentCoordinatorEntry *entry = &owner->entries[index];
  int overridden = id != NULL, changed = overridden != entry->has_language_override ||
                                         !LanguageIdentitySame(&chosen, &review->effective);
  UmiDocumentLanguageReview next = *review;
  if (changed) {
    if (owner->save_in_progress || owner->reopen_in_progress || owner->navigation_depth)
      return UMI_STATUS_BUSY;
    if (entry->language_revision == UINT64_MAX)
      return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(owner->workbench);
    UmiUiDocumentViewSnapshot view;
    status = umi_ui_document_view_model_find(views, entry->view_id, &view);
    if (status != UMI_STATUS_OK)
      return status;
    (void)snprintf(view.language_id, sizeof view.language_id, "%s", chosen.language_id);
    (void)snprintf(view.icon_name, sizeof view.icon_name, "%.*s", (int)sizeof view.icon_name - 1,
                   chosen.icon_name);
    /* Metadata Upsert preserves the full authoritative draft, including text
     * larger than the preview buffer. Calling refresh_view here would copy the
     * stored revision over unsynchronised typing and is deliberately avoided. */
    status = umi_ui_document_view_model_upsert(views, &view);
    if (status != UMI_STATUS_OK)
      return status;
    entry->has_language_override = overridden;
    entry->language_override = chosen;
    ++entry->language_revision;
    next.overridden = overridden;
    next.effective = chosen;
    next.revision = entry->language_revision;
  }
  if (out != NULL)
    *out = next;
  return UMI_STATUS_OK;
}
