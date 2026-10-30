/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/tab_strip.h
 *
 * PURPOSE:
 *   Keep keyed document buttons stable across native presentation refreshes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_WORKSTATION_TAB_STRIP_H
#define UMICOM_UI_GTK4_WORKSTATION_TAB_STRIP_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_GTK4_TAB_STRIP_MAX 256U

typedef struct UmiGtk4TabStripItem {
    const char *id;
    const char *title;
    const char *tooltip;
    int selected;
} UmiGtk4TabStripItem;
typedef void (*UmiGtk4TabStripActivate)(const char *id, void *context);

/** Create a floating horizontal scroller. It owns context after success and
 * calls release_context at finalization. The handler is invoked on the GTK
 * thread with a copied identifier, valid for that call. It may retire the host.
 * Other than explicit button activation, sync never invokes this handler. */
GtkWidget *UmiGtk4TabStripCreate(UmiGtk4TabStripActivate activate,
    void *context, GDestroyNotify release_context);

/** Reconcile a complete ordered snapshot without replacing surviving buttons.
 * Input is copied before notification-capable work. IDs are unique nonempty
 * UTF-8 strings (255 bytes maximum); titles 511 bytes, tooltips 4095 bytes.
 * At most one selected item is allowed. Invalid input leaves the strip intact.
 * Changing the selected ID reveals that button after native allocation without
 * moving focus or activating it. Refreshing the same selection preserves manual
 * scrolling. Deferred reveal stops on retirement and has a bounded frame budget.
 * Reentry returns BUSY. Retirement or external reparenting during a notification
 * returns CANCELLED; a partially presented snapshot may remain until next sync.
 * Use only on the GTK owning thread. NULL items is valid when count is zero. */
UmiStatus UmiGtk4TabStripSync(GtkWidget *strip,
    const UmiGtk4TabStripItem *items, size_t count);

/** Borrow the current button for an ID, or NULL. The strip must stay alive.
 * A caller retaining this button must still check its current ownership. */
GtkWidget *UmiGtk4TabStripFind(GtkWidget *strip, const char *id);

/** Retire dispatch before releasing a borrowed application owner. Safe to call
 * repeatedly. Retained old buttons cannot activate a retired or replaced host.
 * Context remains owned until finalization, including during an active call. */
void UmiGtk4TabStripInvalidate(GtkWidget *strip);
#ifdef __cplusplus
}
#endif
#endif
