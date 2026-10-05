/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/quick_open.h
 * PURPOSE: Present complete, revision-aware indexed filenames without duplicating index or document ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_QUICK_OPEN_H
#define UMICOM_UI_GTK4_QUICK_OPEN_H
#include <gtk/gtk.h>
#include "umicom/platform/file_index.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /** Main-thread callbacks. Read copies one coherent index page without disk
 * enumeration. Open must revalidate the workspace, index identity and selected
 * row (UmiQuickOpenCheckSelection is provided for this), then use the existing
 * document owner. Callbacks may close the dialog; they must not free userData.
 * Inputs are borrowed only for the duration of each call. */
    typedef UmiStatus (*UmiGtk4QuickOpenRead)(gpointer userData, const char *query, size_t offset,
                                              uint64_t expectedRevision, UmiFileIndexEntry *entries,
                                              size_t capacity, UmiFileIndexPage *page);
    typedef UmiStatus (*UmiGtk4QuickOpenOpen)(gpointer userData, const char *query,
                                              const UmiFileIndexPage *page, size_t position,
                                              const UmiFileIndexEntry *entry);
    /** Create, but do not present, a transient file picker. On success it owns
 * userData and calls destroyData once on finalization. On failure ownership
 * remains with the caller and outWindow is NULL. The returned window has a
 * caller-owned reference; destroy it when finished and release that reference.
 * Parent closure retires the picker even when either window is retained.
 * Queries are at most 256 Unicode characters / 1024 UTF-8 bytes. Results are
 * complete paths in pages of 64. Changing the query invalidates previous rows
 * immediately; idle work only reads the index and never opens a file. */
    UmiStatus UmiGtk4QuickOpenCreate(GtkWindow *parent, UmiGtk4QuickOpenRead read, UmiGtk4QuickOpenOpen open,
                                     gpointer userData, GDestroyNotify destroyData, GtkWindow **outWindow);
#ifdef __cplusplus
}
#endif
#endif
