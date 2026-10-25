/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/editor_indentation.h
 *
 * PURPOSE:
 *   Refresh native source views from the document owner's copied indentation policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_EDITOR_INDENTATION_H
#define UMICOM_UI_GTK4_EDITOR_INDENTATION_H
#include "umicom/ui/gtk4.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Owner-thread presentation refresh. Holds native views independently before
 * notifying property observers, and stops when the editing binding retires.
 * No text, document history or settings file changes. An unbound adapter is
 * an unchanged success so ordinary non-document workbenches remain supported. */
UmiStatus UmiGtk4AdapterRefreshIndentation(UmiGtk4Adapter *adapter);
#ifdef __cplusplus
}
#endif
#endif
