/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/indentation_override.h
 *
 * PURPOSE:
 *   Review and apply document-specific indentation without changing other working copies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_INDENTATION_OVERRIDE_H
#define UMICOM_DOCUMENT_INDENTATION_OVERRIDE_H
#include "umicom/document/indentation.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/* A runtime-only review of a named working copy. Keep its coordinator alive:
 * owner is an identity check, not an ownership reference or a serialized token.
 * Text/cursor edits do not stale this review. Default/override changes do,
 * including a change away and back. This is owner-thread state, not thread-safe. */
typedef struct UmiDocumentIndentationReview {
  const UmiDocumentCoordinator *owner;
  UmiDocumentId document_id;
  uint64_t defaults_revision;
  uint64_t override_revision;
  UmiEditorIndentation defaults;
  UmiEditorIndentation effective;
  int overridden;
} UmiDocumentIndentationReview;
/* Canonical managed projections only. Failed reads leave output unchanged. */
UmiStatus UmiDocumentCoordinatorInspectIndentation(const UmiDocumentCoordinator *coordinator,
                                                   UmiDocumentId document_id,
                                                   UmiDocumentIndentationReview *out_review);
/* Compare a previous observation without changing text, selection or policy. */
UmiStatus UmiDocumentCoordinatorCheckIndentation(const UmiDocumentCoordinator *coordinator,
                                                 const UmiDocumentIndentationReview *review);
/* Copy a valid override, or pass NULL to inherit the session defaults. The
 * review must still match even for an otherwise identical no-op. Read-only text
 * can have different indentation presentation. Changed policies are refused
 * during save/reload/navigation. No history entry, dirty flag, preference file
 * or source file is written. Optional output updates only on success.
 * Overrides live until the working copy closes; they are not persisted. */
UmiStatus UmiDocumentCoordinatorApplyIndentation(UmiDocumentCoordinator *coordinator,
                                                 const UmiDocumentIndentationReview *review,
                                                 const UmiEditorIndentation *override,
                                                 UmiDocumentIndentationReview *out_review);
#ifdef __cplusplus
}
#endif
#endif
