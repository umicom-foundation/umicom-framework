/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/document_identity.h
 *
 * PURPOSE:
 *   Bind existing views to owned documents without replacing their source text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_DOCUMENT_IDENTITY_H
#define UMICOM_UI_DOCUMENT_IDENTITY_H
#include "umicom/ui/document_view.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiDocumentIdentityBinding {
    char view_id[UMI_UI_ID_CAPACITY];
    char expected_document_id[UMI_UI_ID_CAPACITY];
    char document_id[UMI_UI_ID_CAPACITY];
} UmiUiDocumentIdentityBinding;

/* Atomically bind existing views at a captured model revision. The caller owns
 * the fixed-size bindings and serializes mutations with its document owner.
 * An empty expected identity is permitted for a contributed, unbound view.
 * New identities must be valid nonempty UI IDs; each view appears at most once.
 * All rows are checked before any mutation. BUSY denotes stale model or view
 * identity, NOT_FOUND a missing view, and INVALID_ARGUMENT malformed bindings.
 * No source, cursor, selection, URI, dirty flag or layout property is changed.
 * A changed identity advances its text revision; a changed batch advances the
 * model revision once. A no-op batch preserves both. No allocation occurs. */
UmiStatus UmiUiDocumentViewModelBindIdentities(UmiUiDocumentViewModel *model,
    uint64_t expectedRevision, const UmiUiDocumentIdentityBinding *bindings, size_t count);
#ifdef __cplusplus
}
#endif
#endif
