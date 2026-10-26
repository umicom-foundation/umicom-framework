/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/kanban_completion.h
 *
 * PURPOSE:
 *   Host explicit private review, retained evidence and owner-checked completion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_UI_GTK4_KANBAN_COMPLETION_H
#define UMICOM_UI_GTK4_KANBAN_COMPLETION_H
#include "umicom/kanban/completion_plan.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiKanbanCompletionReview {
    char reviewer[UMI_KANBAN_ID_SIZE];
    char source_identity[256];
    char configuration[128];
    char evidence_sha256[UMI_SHA256_HEX_CAPACITY];
    char rationale[1024];
    UmiKanbanCompletionDecision decision;
} UmiKanbanCompletionReview;
typedef UmiStatus (*UmiKanbanCompletionCapture)(gpointer context,
    const UmiKanbanCompletionReview *review, UmiKanbanCompletionPlan **out);
typedef UmiStatus (*UmiKanbanCompletionApply)(gpointer context,
    const UmiKanbanCompletionPlan *plan);
/* Capture and Apply run on the GTK owner thread after explicit mapped actions.
 * Capture transfers an independent plan identifying the selected cards. Apply
 * borrows a retained plan for the call and must recheck its current board.
 * Only successful save/load admits a plan to the review preview; Apply requires
 * a separate approval checkbox and click. Loading alone never edits a board.
 *
 * File operations run on workers using owned plan/path copies. Retiring or
 * hiding the panel suppresses late presentation; an already dispatched write
 * can still complete and its file remains. A failed read/write retains the
 * previous accepted plan. Edited entry fields cannot change a captured plan.
 *
 * The panel owns context after successful Create and calls destroy once.
 * Retire before releasing the callback's model; SetBusy before worker access
 * to that model. All public calls are on the GTK owner thread. Files, rationale
 * and preview are private; this control performs no public export or upload. */
GtkWidget *UmiKanbanCompletionGtk4Create(UmiKanbanCompletionCapture capture,
    UmiKanbanCompletionApply apply, gpointer context, GDestroyNotify destroy);
void UmiKanbanCompletionGtk4SetBusy(GtkWidget *panel, gboolean busy);
void UmiKanbanCompletionGtk4Retire(GtkWidget *panel);
gboolean UmiKanbanCompletionGtk4Pending(GtkWidget *panel);
#ifdef __cplusplus
}
#endif
#endif
