/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/source_selection.h
 *
 * PURPOSE:
 *   Expose shared source selection through the native document editing lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_SOURCE_SELECTION_H
#define UMICOM_UI_GTK4_SOURCE_SELECTION_H
#include "umicom/document/source_selection.h"
#include "umicom/ui/gtk4/document_commands.h"
#ifdef __cplusplus
extern "C" {
#endif
/* GTK owner thread. Resolve the current source through its coordinator, then
 * refresh native selection and notify the existing editing completion callback.
 * No text edit, clipboard, source-file or settings write occurs. */
UmiStatus UmiGtk4AdapterSelectSourceUnit(UmiGtk4Adapter *adapter,
                                         UmiEditorSourceSelectionKind kind);
#ifdef __cplusplus
}
#endif
#endif
