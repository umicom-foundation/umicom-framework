/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/kanban_publication_store.h
 *
 * PURPOSE:
 *   Host private publication-plan persistence without granting publication or board-edit authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_KANBAN_PUBLICATION_STORE_H
#define UMICOM_UI_GTK4_KANBAN_PUBLICATION_STORE_H
#include "umicom/kanban/publication_plan.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef UmiStatus (*UmiKanbanPublicPlanCapture)(gpointer context, UmiKanbanPublicationPlan **out);
typedef UmiStatus (*UmiKanbanPublicPlanAdopt)(gpointer context,
                                              const UmiKanbanPublicationPlan *plan);
/* Callbacks run only on the GTK thread after a mapped explicit action. Capture
 * transfers an independent immutable plan to the widget. Adopt borrows the
 * loaded plan for that call and must recheck board identity/revision/policy.
 * Loading alone never calls Adopt. The user reviews all displayed fields and
 * checks approval before adoption; no HTML is generated or uploaded here.
 * The widget owns context after successful creation and calls destroy once.
 * Pass SetBusy before the host accesses borrowed model state on a worker.
 * Retire before releasing that model; pending I/O owns only plan/path copies.
 * All public calls are on the GTK owner thread. */
GtkWidget *UmiKanbanPublicPlanGtk4Create(UmiKanbanPublicPlanCapture capture,
                                         UmiKanbanPublicPlanAdopt adopt, gpointer context,
                                         GDestroyNotify destroy);
void UmiKanbanPublicPlanGtk4SetBusy(GtkWidget *panel, gboolean busy);
void UmiKanbanPublicPlanGtk4Retire(GtkWidget *panel);
gboolean UmiKanbanPublicPlanGtk4Pending(GtkWidget *panel);
#ifdef __cplusplus
}
#endif
#endif
