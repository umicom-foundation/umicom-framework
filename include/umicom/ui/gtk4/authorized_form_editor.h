/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/authorized_form_editor.h
 *
 * PURPOSE:
 *   Native typed editing with current-time, authorized background persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native typed editing with current-time, authorized background persistence. */
#ifndef UMICOM_UI_GTK4_AUTHORIZED_FORM_EDITOR_H
#define UMICOM_UI_GTK4_AUTHORIZED_FORM_EDITOR_H
#include "umicom/data/record_host_clock.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAuthorizedFormEditorGtk4Options {
  UmiRecordHost *host;
  const UmiStructuredFormSchema *schema;
  const char *session_id;
  const char *purpose;
} UmiAuthorizedFormEditorGtk4Options;
typedef struct UmiAuthorizedFormEditorGtk4State {
  bool ready, has_baseline, busy, retired, can_close;
  uint64_t generation, record_revision;
  char entity_id[64];
  UmiStatus last_status;
} UmiAuthorizedFormEditorGtk4State;
/* GTK owner thread only. Returns a floating widget. The host is retained;
 * immutable schema data stays borrowed until widget and all workers finish.
 * The application authenticates and supplies a private current-time session.
 * Session/purpose bindings are fixed for this widget; no actor entry exists.
 * Keep the host's trusted clock state alive through its final reference. */
UmiStatus UmiAuthorizedFormEditorGtk4New(const UmiAuthorizedFormEditorGtk4Options *options,
                                         GtkWidget **out);
/* Begin makes a local unsaved draft; authority is checked at save. Open queues
 * a current-head read. Neither replaces unsaved input. Later visible edits
 * refuse an arriving open result rather than being overwritten. */
UmiStatus UmiAuthorizedFormEditorGtk4Begin(GtkWidget *widget, const char *entity_id);
UmiStatus UmiAuthorizedFormEditorGtk4Open(GtkWidget *widget, const char *entity_id);
/* Save applies valid visible input, captures its immutable typed snapshot, and
 * queues an authorized current-time operation. A clean saved form refuses a
 * duplicate amendment with INVALID_STATE. Editing may continue while it
 * runs. submitting checks required answers; it does not create a product
 * submission/approval event. Reason is copied and observed time is optional. */
UmiStatus UmiAuthorizedFormEditorGtk4Save(GtkWidget *widget, const char *reason,
                                          uint64_t observed_at_nanoseconds, bool submitting);
/* Explicit discard restores the last saved form, or closes a never-saved draft.
 * Busy/retired widgets refuse mutation. Old retained controls are retired. */
UmiStatus UmiAuthorizedFormEditorGtk4Discard(GtkWidget *widget);
/* Snapshot copies the last accepted native values, not incomplete visible text.
 * State remains readable after retirement/disposal and reports actual completed
 * write status. Routine close is allowed only when clean, idle and live. */
UmiStatus UmiAuthorizedFormEditorGtk4Snapshot(GtkWidget *widget, UmiStructuredForm *out);
/* The accessor uses a distinct ordinary C identifier from its state typedef.
 * Keep the original declaration disabled for review of this API correction. */
#if 0
UmiStatus UmiAuthorizedFormEditorGtk4State(GtkWidget *widget,
                                           UmiAuthorizedFormEditorGtk4State *out);
#endif
UmiStatus UmiAuthorizedFormEditorGtk4ReadState(GtkWidget *widget,
                                           UmiAuthorizedFormEditorGtk4State *out);
bool UmiAuthorizedFormEditorGtk4CanClose(GtkWidget *widget);
bool UmiAuthorizedFormEditorGtk4Busy(GtkWidget *widget);
/* Permanent. Does not retire a shared host or cancel/undo an admitted write.
 * Complete the main-context callbacks before releasing schema or clock state.
 * No automatic file recovery or login is provided by this component. */
void UmiAuthorizedFormEditorGtk4Retire(GtkWidget *widget);
#ifdef __cplusplus
}
#endif
#endif
