/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/kanban.h
 * PURPOSE: Present native planning edits and reviewed public snapshots through shared GTK4 controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_KANBAN_H
#define UMICOM_UI_GTK4_KANBAN_H
#include <gtk/gtk.h>
#include <stdbool.h>
#include "umicom/kanban/board.h"
#ifdef __cplusplus
extern "C" {
#endif
/* The panel borrows a board owned by the host/document. All calls are on the GTK
 * owner thread. Retire before destroying that board or releasing host services.
 * The host must not replace/destroy individual panel children. Refresh after
 * external edits; displayed revisions reject stale actions rather than retry.
 * SetBusy(true) before a worker accesses the board/document, and false only
 * after it has completed. Frozen and retired retained controls cannot mutate.
 * This widget does no board-file I/O. Public HTML export uses the shared native
 * text-export control and only fields explicitly entered for public review. */
GtkWidget *UmiKanbanGtk4Create(UmiKanbanBoard *board);
UmiStatus UmiKanbanGtk4Refresh(GtkWidget *panel);
void UmiKanbanGtk4SetBusy(GtkWidget *panel, bool busy);
void UmiKanbanGtk4Retire(GtkWidget *panel);
#ifdef __cplusplus
}
#endif
#endif
