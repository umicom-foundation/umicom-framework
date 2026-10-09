/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/working_tree_gtk4.h
 * PURPOSE: Present shared Git inspection with cancellable work and bounded pages.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_WORKING_TREE_GTK4_H
#define UMICOM_VCS_WORKING_TREE_GTK4_H
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * Create a floating GTK box for read-only, cancellable repository inspection.
     * Copy repository_root now; no Git process starts until the user selects Inspect.
     * The panel owns its queue and drains cancellation through the GTK main context after close.
     * Keep the main context running while background work is pending. Use on the GTK owning thread.
     * This adapter requires nonblocking native joins (currently Windows and non-Android Linux).
     * Other hosts return NOT_IMPLEMENTED; the toolkit-neutral reader and job remain available.
     * Return the box through out_widget only on success; the caller parents or sinks it.
     */
    UmiStatus UmiVcsWorkingTreeGtk4Create(const char *repository_root, void **out_widget);
#ifdef __cplusplus
}
#endif
#endif
