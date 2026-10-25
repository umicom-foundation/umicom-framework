/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/document_language.h
 *
 * PURPOSE:
 *   Review document language changes using stable document identity and the native editing
 * lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_DOCUMENT_LANGUAGE_H
#define UMICOM_UI_GTK4_DOCUMENT_LANGUAGE_H
#include "umicom/document/language_override.h"
#include "umicom/ui/gtk4/document_commands.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Open one modeless review per editing binding. id=0 captures the active tab.
 * Apply always targets the captured working copy, even after switching tabs.
 * Restore filename detection is explicit; close/cancel changes no policy.
 * Results use the existing edit completion callback. No server is started and
 * old language-service observations become stale through their identity check.
 * A retired binding makes retained controls inert. Owner-thread only. */
UmiStatus UmiGtk4AdapterReviewDocumentLanguage(UmiGtk4Adapter *adapter, UmiDocumentId id);
#ifdef __cplusplus
}
#endif
#endif
