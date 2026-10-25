/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/selection_replace.inc
 *
 * PURPOSE:
 *   Compose range-limited search with the existing selected-code proposal and Undo owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/selection_replace.h"
#include "umicom/editor/search_range.h"
UmiStatus UmiDocumentPrepareSelectionReplacement(UmiDocumentCoordinator *owner, UmiDocumentId id,
                                                 const char *needle, const char *replacement,
                                                 const UmiEditorSearchOptions *options,
                                                 UmiDocumentProposal **out, size_t *out_count) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  if (owner == NULL || id == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t needle_bytes = 0U, replacement_bytes = 0U;
  UmiStatus status = ReplacementInputLength(needle, &needle_bytes);
  if (status == UMI_STATUS_OK)
    status = ReplacementInputLength(replacement, &replacement_bytes);
  if (status != UMI_STATUS_OK)
    return status;
  if (needle_bytes == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiEditorSearchOptions policy;
  status = DocumentSearchOptions(options, &policy);
  if (status != UMI_STATUS_OK)
    return status;
  UmiDocumentProposal *proposal = NULL;
  status = UmiDocumentProposalCreate(owner, id, &proposal);
  if (status != UMI_STATUS_OK)
    return status;
  const UmiDocumentEditPlan *capture = proposal->target->capture;
  char identity[UMI_UI_ID_CAPACITY];
  (void)snprintf(identity, sizeof identity, "document.%llu", (unsigned long long)id);
  size_t start = proposal->summary.selection_offset, span = proposal->summary.selection_bytes;
  if (strcmp(identity, capture->view.document_id) || start > capture->length ||
      span > capture->length - start)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK &&
      (!umi_document_utf8_validate((const unsigned char *)capture->text + start, span, NULL) ||
       (start > 0U && start < capture->length && capture->text[start] == '\n' &&
        capture->text[start - 1U] == '\r') ||
       (start + span > 0U && start + span < capture->length &&
        capture->text[start + span] == '\n' && capture->text[start + span - 1U] == '\r')))
    status = UMI_STATUS_INVALID_ARGUMENT;
  size_t bytes = 0U, count = 0U;
  char *text = NULL;
  if (status == UMI_STATUS_OK)
    status =
        UmiEditorSearchReplaceRangeSize(capture->text, capture->length, start, span, needle,
                                        needle_bytes, replacement_bytes, &policy, &bytes, &count);
  if (status == UMI_STATUS_OK && bytes > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES)
    status = UMI_STATUS_CAPACITY_EXCEEDED;
  if (status == UMI_STATUS_OK) {
    text = malloc(bytes + 1U);
    if (text == NULL)
      status = UMI_STATUS_OUT_OF_MEMORY;
  }
  if (status == UMI_STATUS_OK)
    status = UmiEditorSearchReplaceRange(capture->text, capture->length, start, span, needle,
                                         needle_bytes, replacement, replacement_bytes, &policy,
                                         text, bytes + 1U, NULL);
  if (status == UMI_STATUS_OK) {
    size_t selected_bytes = bytes - (capture->length - span);
    status = UmiDocumentProposalSetText(proposal, proposal->summary.revision, text + start,
                                        selected_bytes);
  }
  free(text);
  if (status != UMI_STATUS_OK) {
    UmiDocumentProposalDestroy(proposal);
    return status;
  }
  *out = proposal;
  if (out_count != NULL)
    *out_count = count;
  return UMI_STATUS_OK;
}
