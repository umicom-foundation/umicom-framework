/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/recovery_coordinator.h
 * PURPOSE: Capture visible drafts and restore recovered source into a separate unsaved working copy.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_RECOVERY_COORDINATOR_H
#define UMICOM_DOCUMENT_RECOVERY_COORDINATOR_H
#include "umicom/document/recovery_draft.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Capture complete visible source, including typing not yet synchronized to
 * DocumentStore. Do not save, synchronize, clear dirty state or alter history.
 * key is a new identity from RecoveryKeyCreate. Capture on the owning thread,
 * then hand the independent draft to a storage worker. source_revision describes
 * the underlying store at capture; it is not a hash of unsynchronized typing.
 * Failure clears out_draft. Read-only documents can also be captured. */
    UmiStatus UmiDocumentCoordinatorCaptureRecovery(UmiDocumentCoordinator *coordinator,
                                                    UmiDocumentId document_id, const char *key,
                                                    const UmiCancellationToken *cancel,
                                                    UmiDocumentRecoveryDraft **out_draft);
    /* Restore the validated draft as a NEW unsaved document, activate it, and
 * restore its selection. Existing source paths, tabs, text and histories are
 * never overwritten. Original path metadata does not authorize a file write;
 * the user must explicitly choose Save As. This creates no synthetic Undo step
 * which could erase the recovered source. Repeated restore creates separate
 * working copies. Failure clears out_document and leaves no partial new tab.
 * No file is read or written. Use the coordinator's owning thread. */
    UmiStatus UmiDocumentCoordinatorRestoreRecovery(UmiDocumentCoordinator *coordinator,
                                                    const UmiDocumentRecoveryDraft *draft,
                                                    UmiDocumentId *out_document);
#ifdef __cplusplus
}
#endif
#endif
