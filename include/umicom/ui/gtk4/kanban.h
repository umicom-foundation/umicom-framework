/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/kanban.h
 *
 * PURPOSE:
 *   Present native planning edits and reviewed public snapshots through shared GTK4 controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
 * text-export control and only fields explicitly entered for public review.
 * The optional completion-review section captures one selected card, retains
 * a private .umicompletion file and requires explicit approval before applying
 * a checked Done move. It uses the first configured Done-stage column. The
 * host remains responsible for saving the board after a successful edit.
 * The private card-details form explicitly loads title, description, priority
 * and blocker at the selected revision. Applying retains every other card
 * field. Refresh never authorizes a stale form; selecting a different card,
 * hiding the form or freezing its host requires a new explicit load.
 * The separate acceptance checklist form replaces only reviewed checklist
 * rows under the same revision guard. Empty text explicitly removes an item;
 * checkmarks are user assertions, not authenticated qualification evidence.
 * Prerequisite selectors load exact card IDs and board revision before explicit
 * Add/Remove. Prompt rows perform no edit, and native display order never
 * replaces the captured IDs. Existing cycle and Done rules remain in force.
 * Labels load an active card at its selected revision, preserve all other fields
 * and stay private. Empty rows are omitted and duplicate labels are refused.
 * List limits load a named column and active occupancy; zero is unlimited. A
 * different picker selection, even away-and-back, invalidates the review.
 * Limit changes cannot silently move cards or lower a nonzero limit below
 * occupancy. These forms require a new load after freezing or hiding.
 * Deadline edits require exact UTC seconds in YYYY-MM-DDTHH:MM:SSZ notation
 * and an explicit Use deadline checkbox. Clearing is deliberate; unsupported
 * larger portable values are preserved until explicitly replaced or cleared.
 * Native dates start one second after the Unix epoch and end in year 9999.
 * No scheduling, timezone inference, reminder or automatic completion occurs.
 * The archive form loads active and archived cards into a revision-bound picker.
 * Review describes the exact archive/restore action; Apply performs it once.
 * Selection changes, reloads, hidden forms and busy hosts invalidate reviews.
 * Restoring uses the recorded list and existing WIP/dependency rules. Archiving
 * never deletes fields, releases board capacity or publishes private text.
 * Native filters reuse C23 queries; they affect only active-card presentation.
 * Developer names are case-folded, exact registered IDs take precedence, and
 * ambiguous names are refused. List occupancy remains unfiltered. A new filter
 * abandons card selection and drag authority without editing board records.
 * The overdue cutoff is explicit UTC; no host clock is consulted. */
GtkWidget *UmiKanbanGtk4Create(UmiKanbanBoard *board);
UmiStatus UmiKanbanGtk4Refresh(GtkWidget *panel);
void UmiKanbanGtk4SetBusy(GtkWidget *panel, bool busy);
void UmiKanbanGtk4Retire(GtkWidget *panel);
#ifdef __cplusplus
}
#endif
#endif
