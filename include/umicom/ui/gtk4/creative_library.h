/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/creative_library.h
 * PURPOSE: Compose an explicit native asset library with worker-owned storage and a reusable asset editor.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_CREATIVE_LIBRARY_H
#define UMICOM_UI_GTK4_CREATIVE_LIBRARY_H
#include <gtk/gtk.h>
#include "umicom/creative_workspace/asset_library.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Floating widget. Construction opens no files. Pass an existing Assets editor
 * to enable the explicit Copy to Assets editor action, or NULL. The editor is
 * weakly referenced and receives an independent capture, never a library borrow.
 * One library is owned until closed. Save/Open are explicit; scene project and
 * library storage remain separate. Retained controls become inert after close. */
    GtkWidget *UmiCreativeAssetLibraryGtkCreate(GtkWidget *asset_editor);
    /* Snapshot calls do not retain the owner and return BUSY while a worker owns it.
 * They are intended for host integration and native acceptance checks. */
    UmiStatus UmiCreativeAssetLibraryGtkInspect(GtkWidget *root, UmiCreativeAssetLibraryInfo *out);
    UmiStatus UmiCreativeAssetLibraryGtkAt(GtkWidget *root, size_t index, UmiCreativeAssetLibraryEntry *out);
#ifdef __cplusplus
}
#endif
#endif
