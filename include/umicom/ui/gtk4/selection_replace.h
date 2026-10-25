/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/selection_replace.h
 *
 * PURPOSE:
 *   Review literal replacements within the captured selection through the existing selected-code
 * approval window.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_SELECTION_REPLACE_H
#define UMICOM_UI_GTK4_SELECTION_REPLACE_H
#include "umicom/document/selection_replace.h"
#include "umicom/ui/gtk4/document_commands.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Capture the active source selection and prefill its proposed replacement.
 * Requires a writable nonempty selection. Nothing is applied on opening:
 * Preview complete draft, explicit approval and Apply remain separate actions.
 * Users may edit the prefilled proposal; doing so invalidates any preview.
 * Whole-word checks use full-source context. The initial literal options are
 * copied by preparation and never read again from application widgets.
 * Same lifetime, busy, completion and cancellation rules as ReviewSelectedCode. */
UmiStatus UmiGtk4AdapterReviewSelectionReplacement(UmiGtk4Adapter *adapter, const char *needle,
                                                   const char *replacement,
                                                   const UmiEditorSearchOptions *options);
#ifdef __cplusplus
}
#endif
#endif
