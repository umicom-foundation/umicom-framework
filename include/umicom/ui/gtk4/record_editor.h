/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A reusable native editor for bounded local record notes. Storage and draft
 * ownership remain in Framework; products supply their title and namespace. */
#ifndef UMICOM_UI_GTK4_RECORD_EDITOR_H
#define UMICOM_UI_GTK4_RECORD_EDITOR_H
#include <gtk/gtk.h>
#include <stdbool.h>
#include "umicom/data/record_draft.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRecordEditorGtk4Options {
    const char *title;
    const char *description;
    const char *entity_prefix;
    const char *source;
} UmiRecordEditorGtk4Options;

/* Returns a floating widget on success and leaves output unchanged on failure.
 * Strings are copied. entity_prefix is nonempty, at most 48 bytes and reserves
 * a product namespace; the entered key and prefix together must fit 63 bytes.
 * source is a nonempty provenance label of at most 159 bytes, not authentication.
 *
 * The user explicitly opens or creates one local SQLite database per widget.
 * Database calls run on GTask workers. The widget never contacts a network.
 * UTF-8 text without embedded NUL is supported; arbitrary binary documents are
 * refused without replacing the current draft. Content is not encrypted.
 *
 * Call all widget APIs on the GTK owning thread. Unroot, dispose and Retire
 * revoke action authority even when a host retains child controls. Accepted
 * writes are not cancelled by retirement; keep the process and owning main
 * context alive until Busy becomes false. Late reads cannot restore a retired
 * editor. Forced process exit cannot guarantee completion. Recreate the editor
 * to attach it to another root or choose a different database.
 *
 * Compare reads a separate stored snapshot while local typing remains available.
 * Explicit adoption changes only the baseline after review; it does not merge
 * text, save it or bypass the next storage revision check. */
UmiStatus UmiRecordEditorGtk4New(const UmiRecordEditorGtk4Options *options, GtkWidget **output);
bool UmiRecordEditorGtk4Busy(GtkWidget *editor);
UmiStatus UmiRecordEditorGtk4State(GtkWidget *editor, UmiRecordDraftState *output);
/* CanClose includes invalid oversized visible text as unsaved work. RequestClose
 * also displays a reason when it refuses. Hosts should return the inverse from
 * close-request; forced shutdown still cannot guarantee unsaved-draft recovery. */
bool UmiRecordEditorGtk4CanClose(GtkWidget *editor);
bool UmiRecordEditorGtk4RequestClose(GtkWidget *editor);
void UmiRecordEditorGtk4Retire(GtkWidget *editor);
#ifdef __cplusplus
}
#endif
#endif
