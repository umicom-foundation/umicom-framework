/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/document_indentation.h
 *
 * PURPOSE:
 *   Open a reusable document-scoped indentation review through the existing editing binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_DOCUMENT_INDENTATION_H
#define UMICOM_UI_GTK4_DOCUMENT_INDENTATION_H
#include "umicom/document/indentation_override.h"
#include "umicom/ui/gtk4.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Open a modeless form for a captured working copy; zero selects the active
 * document at invocation. One form per editing binding. Applying affects that
 * document even if another tab becomes active. Stale settings/closed documents
 * are rejected. Rebinding invalidates mutation; closing the parent closes the
 * form. No file writes or automatic settings adoption. GTK owner thread only.
 * OK means the form opened, not that preferences were applied. */
UmiStatus UmiGtk4AdapterReviewDocumentIndentation(UmiGtk4Adapter *adapter,
                                                  UmiDocumentId document_id);
#ifdef __cplusplus
}
#endif
#endif
