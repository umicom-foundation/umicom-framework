/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/document_store_edits.h
 * PURPOSE: Commit complete, checked sets of document replacements under one store lock.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_DOCUMENT_STORE_EDITS_H
#define UMICOM_PLATFORM_DOCUMENT_STORE_EDITS_H
#include "umicom/platform/document_store.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_DOCUMENT_STORE_EDIT_MAXIMUM_BYTES (8U * 1024U * 1024U)
#define UMI_DOCUMENT_STORE_EDIT_BUDGET_BYTES (64U * 1024U * 1024U)

    /* Each expected snapshot must come from this store. All input is borrowed for
 * the call and must remain stable; zero-length text still needs a valid pointer.
 * The store copies bytes and never takes ownership of the caller's buffers. */
    typedef struct UmiDocumentStoreTextChange
    {
        const UmiDocumentSnapshot *expected;
        const char *text;
        size_t length;
        /* Use 1 when a higher-level draft edit must record a change even if it
     * returns to the stored bytes. Ordinary byte comparisons use 0. */
        int force_revision;
    } UmiDocumentStoreTextChange;

    /* Replace a nonempty set of distinct existing documents, or change none.
 * Allocation and validation precede publication. A stale snapshot, closed ID,
 * repeated ID, embedded zero, capacity limit or allocation failure cannot leave
 * an earlier row applied. Unchanged text retains its revision and conflict flag.
 * Changed text (or force_revision == 1) advances its revision, preserves its saved revision, and clears
 * its external-change flag, as the ordinary replacement operation does.
 * This byte-oriented service does not decode text, write files, update editor
 * views or record Undo. A document coordinator provides those higher-level rules.
 * Calls from different threads serialize on the store's existing mutex. */
    UmiStatus UmiDocumentStoreReplaceTextChanges(UmiDocumentStore *store,
                                                 const UmiDocumentStoreTextChange *changes, size_t count);
#ifdef __cplusplus
}
#endif
#endif
