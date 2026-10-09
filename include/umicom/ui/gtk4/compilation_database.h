/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/compilation_database.h
 * PURPOSE: Offer a reusable read-only compiler database inspector and explicit clangd argument preparation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_COMPILATION_DATABASE_H
#define UMICOM_UI_GTK4_COMPILATION_DATABASE_H
#include "umicom/base/status.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C"
{
#endif
    /** Create a floating panel to parent on the GTK thread. Optional folder/source
 * drafts are copied; the source, when supplied, must be absolute when reading.
 * Opening reads nothing. Read runs bounded parsing on a worker. Editing inputs
 * or hiding the panel invalidates pending results, and closing never waits for
 * a disk read. Rows are metadata, not executable actions.
 *
 * clangd_arguments may be NULL for inspection only. Otherwise a weak reference
 * permits the explicit Use for clangd button to update that entry in the same
 * live window. This prepares argument text only: it does not select an
 * executable, grant trust, persist settings, or launch a server. The host must
 * label the entry as clangd arguments and use its normal reviewed launch path.
 * Retained controls cannot update a removed panel or a different window. */
    UmiStatus UmiGtk4CompilationDatabasePanelCreate(const char *directory, const char *source_file,
                                                    GtkEditable *clangd_arguments, GtkWidget **out);
#ifdef __cplusplus
}
#endif
#endif
