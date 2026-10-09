/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/debug_sources.h
 * PURPOSE: Host a read-only debugger source browser with explicit retrieval and weak owner callbacks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_DEBUG_SOURCES_H
#define UMICOM_UI_GTK4_DEBUG_SOURCES_H
#include "umicom/debug_runtime/source_catalog.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C"
{
#endif
    typedef UmiStatus (*UmiGtk4DebugSourcesRefresh)(void *context, UmiDebugSourceCatalog *out);
    typedef UmiStatus (*UmiGtk4DebugSourcesRead)(void *context,
                                                 const UmiDebugConnectionIdentity *connection,
                                                 uint32_t reference, UmiDebugSourceContent *out);
    /** Create an initially idle source browser; the caller adopts the floating widget.
 * Refresh and View source are explicit actions on the GTK owner thread. Callbacks
 * must recheck host lifetime and current workspace authority. Context ownership
 * transfers only on success and destroy runs once when the panel is finalized.
 * The panel copies replies, renders plain read-only text, and never opens a path.
 * Filtering and pagination use the captured catalogue without extra adapter I/O.
 * Hidden or retained controls cannot initiate new requests. */
    GtkWidget *UmiGtk4DebugSourcesCreate(UmiGtk4DebugSourcesRefresh refresh,
                                         UmiGtk4DebugSourcesRead read, void *context,
                                         GDestroyNotify destroy);
#ifdef __cplusplus
}
#endif
#endif
