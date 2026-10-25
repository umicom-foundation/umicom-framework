/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/structured_form_search_draft.h
 *
 * PURPOSE:
 *   Native query drafts share portable revision, save and recovery ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native query drafts share portable revision, save and recovery ownership. */
#ifndef UMICOM_UI_GTK4_STRUCTURED_FORM_SEARCH_DRAFT_H
#define UMICOM_UI_GTK4_STRUCTURED_FORM_SEARCH_DRAFT_H
#include "umicom/data/structured_form_search_session.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Floating widget, GTK owner thread only. Schema data is borrowed immutable for
 * the widget and outstanding requests' lifetime. No database or file I/O runs
 * here. The host authorizes reads/writes and dispatches copied save requests. */
UmiStatus UmiStructuredFormSearchDraftGtk4New(const UmiStructuredFormSchema *schema,
                                              const char *entity_prefix, GtkWidget **out);
UmiStatus UmiStructuredFormSearchDraftGtk4Begin(GtkWidget *widget, const char *entity_id,
                                                const UmiStructuredFormSearchPreset *preset);
UmiStatus UmiStructuredFormSearchDraftGtk4Open(GtkWidget *widget,
                                               const UmiRecordDocument *document);
/* State returns the last accepted portable model. Visible incomplete text is
 * retained separately. Accept copies valid native text into that model; an
 * invalid title or condition leaves the accepted model and visible text intact.
 * CanClose accepts valid current text before checking dirty/pending state. */
UmiStatus UmiStructuredFormSearchDraftGtk4Accept(GtkWidget *widget);
UmiStatus UmiStructuredFormSearchDraftGtk4State(GtkWidget *widget,
                                                UmiStructuredFormSearchSessionView *out);
bool UmiStructuredFormSearchDraftGtk4CanClose(GtkWidget *widget);
/* Explicit discard restores saved controls or closes a new definition.
 * Removed controls retained elsewhere cannot change the replacement draft. */
UmiStatus UmiStructuredFormSearchDraftGtk4Discard(GtkWidget *widget);
/* Prepare accepts visible values, then captures an immutable request.
 * Finish reports the real worker outcome even after widget retirement/disposal;
 * it never replaces later native edits. Retain the widget while a request is
 * outstanding, keep processing completion, then release it. */
UmiStatus UmiStructuredFormSearchDraftGtk4PrepareSave(GtkWidget *widget,
                                                      const UmiRecordDraftAttribution *attribution,
                                                      UmiRecordSaveRequest *out);
UmiStatus UmiStructuredFormSearchDraftGtk4FinishSave(GtkWidget *widget, uint64_t request_id,
                                                     UmiStatus result,
                                                     const UmiRecordRevision *saved);
UmiStatus UmiStructuredFormSearchDraftGtk4Capture(GtkWidget *widget, UmiRecordDraftSnapshot *out);
/* Check the selected database with record_draft_snapshot_check_store first.
 * Restore refuses unsaved native edits and validates schema/namespace/baseline.
 * It does not select a database or write any stored definition. */
UmiStatus UmiStructuredFormSearchDraftGtk4Restore(GtkWidget *widget,
                                                  const UmiRecordDraftSnapshot *snapshot);
void UmiStructuredFormSearchDraftGtk4Retire(GtkWidget *widget);
#ifdef __cplusplus
}
#endif
#endif
