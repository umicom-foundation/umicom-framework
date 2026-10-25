/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/form_record_editor.h
 *
 * PURPOSE:
 *   Compose typed forms with revision-checked local record persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Compose typed forms with revision-checked local record persistence. */
#ifndef UMICOM_UI_GTK4_FORM_RECORD_EDITOR_H
#define UMICOM_UI_GTK4_FORM_RECORD_EDITOR_H
#include "umicom/application/experience/structured_form_catalogue.h"
#include "umicom/application/experience/structured_form_filter.h"
#include "umicom/data/structured_form_session.h"
#include "umicom/data/structured_form_search_catalogue.h"
#include "umicom/data/structured_form_search_session.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFormRecordEditorGtk4Options {
  const UmiStructuredFormSchema *schema;
  const char *entity_prefix;
  const char *source;
  /* Optional pure domain constraint, evaluated after generic key validation.
   * Must not call GTK, mutate this editor or retain the borrowed key. */
  UmiStatus (*key_validator)(const char *key);
} UmiFormRecordEditorGtk4Options;
/* Returns a floating widget; output is unchanged on failure. The immutable
 * schema is borrowed for the widget and outstanding workers' entire lifetime.
 * Other option strings are copied. Prefix is ASCII letters/digits/._-, ends
 * in '.', and occupies at most 48 bytes. Keys use letters/digits/_-; the full
 * entity identifier must fit 63 bytes. All APIs belong to the GTK owner thread.
 * This editor stores ordinary, unencrypted records, not clinical decisions,
 * payment instructions or credentials. Attribution labels are not login proof.
 * SQLite work runs on GTask workers. A missing SQLite backend reports its
 * unsupported status without silently selecting an in-memory substitute. */
UmiStatus UmiFormRecordEditorGtk4New(const UmiFormRecordEditorGtk4Options *options,
                                     GtkWidget **out);
/* Optionally register historical schema definitions once, before connection.
 * The immutable
 * catalogue and all borrowed schemas must outlive the widget and
 * outstanding work. Its active
 * ID/version must resolve to the options.schema
 * pointer itself. Inspection uses exact old
 * versions; editing still uses only
 * the active schema. This never changes, migrates or writes
 * stored answers. */
UmiStatus UmiFormRecordEditorGtk4SetHistoryCatalogue(GtkWidget *editor,
                                                     const UmiStructuredFormCatalogue *catalogue);
/* Connect creates or opens an explicitly selected absolute SQLite path once.
 * Connect/Open/Save return acceptance; inspect Busy/LastStatus for completion.
 * A failed operation keeps existing answers. Open reads the latest consistent
 * document for the exact key; missing records are never automatically created.
 * Dirty native text blocks opening another record, even if parsing would fail. */
UmiStatus UmiFormRecordEditorGtk4Connect(GtkWidget *editor, const char *path);
/* Find searches bounded metadata in this editor's namespace. A next page uses
 * the previous result snapshot and exact query; changed heads require refresh.
 * OpenListed copies a zero-based result row and verifies its current head before
 * replacing a clean draft. Search text changed during a read rejects its result.
 * History pages belong to the current entity/generation. InspectRevision copies
 * a zero-based history row and displays its verified content read-only; dirty
 * answers remain untouched. All these methods return asynchronous acceptance.
 * Missing/malformed/incompatible content is refused, never silently migrated. */
UmiStatus UmiFormRecordEditorGtk4Find(GtkWidget *editor, const char *query, int next_page);
/* Typed search adds visible AND conditions to the metadata query. SetFilter
 * explicitly replaces query conditions without changing record drafts.
 * FindTyped captures native text, the active schema version and namespace,
 * then verifies saved payloads on a worker. Register historical schemas first
 * if the namespace contains older versions. They are verified but excluded
 * from active-version answer matches. Invalid text or changed query identity
 * refuses results. Continuations require the same mode, revision and metadata.
 * Search may run with unsaved record edits; it searches stored answers only.
 * OpenListed refuses changed filters before and after its asynchronous read.
 * Ordinary Find retains its existing metadata-only behavior. */
UmiStatus UmiFormRecordEditorGtk4SetFilter(GtkWidget *editor, const UmiStructuredFormFilter *filter);
UmiStatus UmiFormRecordEditorGtk4FindTyped(GtkWidget *editor, const char *metadata, int next_page);

/* Saved searches are separate records in form.search.*. SaveSearch copies the
 * current query and attribution, using zero to create or a reviewed revision to
 * amend. It never saves record answers. Accepted saves complete after retirement.
 * OpenSearch replaces query controls only if they have not changed during its
 * read and the stored namespace equals this editor's namespace. It never starts
 * a search automatically and never changes the record draft. Rendering checks
 * UTF-8 before applying a definition. Changes made by synchronous GTK observers
 * remain visible; a changed final query reports cancellation without rollback.
 * SavedSearch returns the last successfully saved/opened metadata, including a
 * completed save after retirement, without changing output when none exists. */
UmiStatus UmiFormRecordEditorGtk4SaveSearch(GtkWidget *editor, const char *key, const char *title,
    uint64_t expected_revision, const UmiRecordDraftAttribution *attribution);
UmiStatus UmiFormRecordEditorGtk4OpenSearch(GtkWidget *editor, const char *key);
UmiStatus UmiFormRecordEditorGtk4SavedSearch(GtkWidget *editor, UmiRecordRevision *out);

/* Saved-definition discovery uses the registered historical catalogue, or the
 * active schema alone when no catalogue was supplied before connecting.
 * It never changes record answers or query conditions. Find owns a worker;
 * Page copies only a completed page still bound to the visible title filter.
 * OpenResult checks the listed head on storage and rejects later selection,
 * title-filter or query-control edits. No automatic refresh or search occurs. */
UmiStatus UmiFormRecordEditorGtk4FindSavedSearches(GtkWidget *editor, const char *text, int next_page);
UmiStatus UmiFormRecordEditorGtk4SavedSearchPage(GtkWidget *editor, UmiStructuredFormSearchPage *out);
UmiStatus UmiFormRecordEditorGtk4OpenSavedSearchResult(GtkWidget *editor, size_t index);

/* Managed query editing has its own draft, independent of scratch query controls
 * and record answers. Begin copies the current query; Edit loads a saved query.
 * Reads, saves and recovery use the same serialized worker as record actions.
 * CanClose now checks both record and managed query drafts. These APIs neither
 * execute a query nor install it into the scratch search controls automatically. */
UmiStatus UmiFormRecordEditorGtk4BeginSearchDraft(GtkWidget *editor, const char *key,
                                                const char *title);
UmiStatus UmiFormRecordEditorGtk4EditSearchDraft(GtkWidget *editor, const char *key);
UmiStatus UmiFormRecordEditorGtk4SearchDraftState(GtkWidget *editor,
                                                UmiStructuredFormSearchSessionView *out);
UmiStatus UmiFormRecordEditorGtk4SaveSearchDraft(GtkWidget *editor,
                                               const UmiRecordDraftAttribution *attribution);
UmiStatus UmiFormRecordEditorGtk4DiscardSearchDraft(GtkWidget *editor);
UmiStatus UmiFormRecordEditorGtk4KeepSearchRecovery(GtkWidget *editor, const char *path);
UmiStatus UmiFormRecordEditorGtk4RecoverSearch(GtkWidget *editor, const char *path);

UmiStatus UmiFormRecordEditorGtk4OpenListed(GtkWidget *editor, size_t index);
UmiStatus UmiFormRecordEditorGtk4History(GtkWidget *editor, int older_page);
UmiStatus UmiFormRecordEditorGtk4InspectRevision(GtkWidget *editor, size_t index);
UmiStatus UmiFormRecordEditorGtk4Begin(GtkWidget *editor, const char *key);
UmiStatus UmiFormRecordEditorGtk4Open(GtkWidget *editor, const char *key);
/* Direct callers supply a positive recorded_at_nanoseconds value. The visible
 * Save buttons
 * capture checked UTC time themselves; observed time may be zero. */
UmiStatus UmiFormRecordEditorGtk4Save(GtkWidget *editor,
                                      const UmiRecordDraftAttribution *attribution, int submitting);
/* An explicit discard decision restores the saved baseline or closes a new
 * record. It cannot run during I/O. Hosts supply their own confirmation UI. */
UmiStatus UmiFormRecordEditorGtk4Discard(GtkWidget *editor);
/* Compare a saved baseline, accepted local answers and the latest checked
 * stored document. Required answers may remain incomplete during review.
 * Changed native input invalidates the comparison. ApplyReview requires all
 * conflicts resolved, changes local baseline/answers only, and never saves. */
UmiStatus UmiFormRecordEditorGtk4ReviewStored(GtkWidget *editor);
UmiStatus UmiFormRecordEditorGtk4ApplyReview(GtkWidget *editor);
/* Explicit plaintext recovery files reuse the checked record snapshot codec.
 * Keep validates visible answers as a draft, creates a NEW file exclusively,
 * and leaves database dirty state unchanged. Invalid visible text stays in the
 * editor and is not truncated into a recovery record. Recover requires clean
 * local state, matching namespace/schema and the selected store baseline.
 * A stale recovered head remains protected by the next save revision check. */
UmiStatus UmiFormRecordEditorGtk4KeepRecovery(GtkWidget *editor, const char *path);
UmiStatus UmiFormRecordEditorGtk4Recover(GtkWidget *editor, const char *path);
/* Export complete saved history to a new explicit plaintext file. Requires a
 * clean saved record,
 * including native text, and rechecks its exact stored head
 * on the worker. Never saves a draft,
 * overwrites a file or changes dirty state.
 * Export returns acceptance; Busy/LastStatus report
 * the completed file write.
 * Accepted writes finish after retirement. Hosts own disclosure
 * authorization. */
UmiStatus UmiFormRecordEditorGtk4ExportHistory(GtkWidget *editor, const char *path);
/* State contains accepted record-session answers, not unapplied native typing.
 * Later typing remains in the native editor when a save completes. */
UmiStatus UmiFormRecordEditorGtk4State(GtkWidget *editor, UmiStructuredFormSessionView *out);
UmiStatus UmiFormRecordEditorGtk4LastStatus(GtkWidget *editor);
bool UmiFormRecordEditorGtk4Busy(GtkWidget *editor);
bool UmiFormRecordEditorGtk4CanClose(GtkWidget *editor);
/* Displays a close refusal while retaining all drafts and pending operations. */
bool UmiFormRecordEditorGtk4RequestClose(GtkWidget *editor);
/* Unroot/dispose/Retire permanently revoke action authority. An accepted write
 * still finishes; retain the main context and process until Busy becomes false.
 * Never treat retirement as cancellation of a write already admitted. */
void UmiFormRecordEditorGtk4Retire(GtkWidget *editor);
#ifdef __cplusplus
}
#endif
#endif
