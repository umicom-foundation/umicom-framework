/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/list_publication.h
 * PURPOSE: Replace native list rows only while their host snapshot remains current.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_LIST_PUBLICATION_H
#define UMICOM_UI_GTK4_LIST_PUBLICATION_H
#include "umicom/base/status.h"
#include <gtk/gtk.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_GTK4_LIST_PUBLICATION_LIMIT 2048U
/** Check the host generation, selection/query and source revision on the GTK
 * owning thread. Context must remain alive through return. Return a non-OK
 * status when the captured page should no longer be displayed. This callback
 * may retire the host; it must not recursively mutate the same native list. */
typedef UmiStatus (*UmiGtk4ListPublicationCurrent)(GtkListBox *list, void *context);
/** Replace the rows of an unbound GtkListBox. The caller owns a strong,
 * nonfloating reference to each distinct unparented widget in rows. Widgets
 * may be GtkListBoxRows or ordinary embeddable children, but never a native
 * root or an ancestor of list; the helper owns temporary
 * references and never takes the caller's ownership. NULL rows is valid only
 * when count is zero. Model binding and sorting must not be active. Apply filtering to the
 * source snapshot before preparing rows, not through native list callbacks.
 *
 * Preflight failure leaves existing rows untouched. Once removal starts,
 * native observers can cancel publication: the list may then contain a partial
 * page. No rollback, automatic retry, focus, selection or activation occurs.
 * The host must invalidate action identities before calling, and mark a page
 * rendered only on OK. Recheck the host before using it after this call.
 *
 * Recursive publication returns BUSY. Counts above the limit return
 * CAPACITY_EXCEEDED. An unexpected row mutation returns INVALID_STATE.
 * All calls, callbacks and object release belong on the GTK owning thread. */
UmiStatus UmiGtk4ListBoxPublish(GtkListBox *list, GtkWidget *const *rows, size_t count,
    UmiGtk4ListPublicationCurrent current, void *context);
#ifdef __cplusplus
}
#endif
#endif
