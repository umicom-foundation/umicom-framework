/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/language_comment.h
 *
 * PURPOSE:
 *   Resolve line-comment policy from the named source view and publish through existing document
 * Undo ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_LANGUAGE_COMMENT_H
#define UMICOM_DOCUMENT_LANGUAGE_COMMENT_H
#include "umicom/document/line_edit.h"
#include "umicom/editor/comment_style.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Observe the canonical named view's current language identity. Read-only
 * inspection is permitted. Unknown language returns NOT_FOUND; closed or
 * mismatched views never fall back to the active tab or C syntax. The returned
 * style is static catalogue data; this observation grants no edit permission.
 * No I/O or source changes occur. Output is unchanged on refusal. */
UmiStatus UmiDocumentCoordinatorLineCommentStyle(UmiDocumentCoordinator *owner, UmiDocumentId id,
                                                 const UmiEditorLineCommentStyle **out);
/** Recheck language, identity and edit admission, then toggle the language's
 * line prefix through the established selected-line plan and one Undo owner.
 * No selection means the caret line; blank lines and existing whitespace
 * follow line_edit_plan.h. Existing literal // commands remain unchanged.
 * Unsynchronized typing keeps a separate Undo step. Nothing is saved and no
 * syntax parser runs. A language token can still be wrong inside a string or
 * embedded language; review the draft. Owner thread only, no callbacks or I/O. */
UmiStatus UmiDocumentCoordinatorToggleLanguageComment(UmiDocumentCoordinator *owner,
                                                      UmiDocumentId id);
#ifdef __cplusplus
}
#endif
#endif
