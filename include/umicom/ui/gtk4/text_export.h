/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/text_export.h
 * PURPOSE: Save an explicit snapshot of displayed text through shared native UI.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_TEXT_EXPORT_H
#define UMICOM_UI_GTK4_TEXT_EXPORT_H
#include <gtk/gtk.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEXT_EXPORT_MAX_CHARACTERS 2097152
#define UMI_TEXT_EXPORT_MAX_BYTES 8388608U

/**
 * @brief Create a floating export control for a label or text view.
 * @param source Borrowed GtkLabel or GtkTextView, observed through a weak reference.
 * @param suggestedFilename Borrowed filename suggestion copied during construction.
 * @return Floating widget, or NULL for an unsupported source or invalid suggestion.
 * @details The user selects an absolute new filename and explicitly saves the
 * currently displayed text. The main thread copies at most the documented limits
 * before any file is created. A worker writes that immutable UTF-8 snapshot.
 * Hidden text is omitted. This is plain text, not a database backup or CSV export.
 * Existing files are never overwritten; no parent directory is created. An I/O
 * failure may leave a partial new file for inspection. Call Retire before a host
 * releases services or leaves retained controls available after closing.
 */
GtkWidget *UmiTextExportGtk4Create(GtkWidget *source, const char *suggestedFilename);

/**
 * @brief Create an export control for a report owned by a text buffer.
 * @param source Borrowed text buffer observed through a weak reference.
 * @param suggestedFilename Filename suggestion copied during construction.
 * @return Floating control, or NULL for an invalid argument.
 * @details Uses the same snapshot, size, file and retirement rules as Create.
 */
GtkWidget *UmiTextExportGtk4CreateForBuffer(GtkTextBuffer *source, const char *suggestedFilename);

/**
 * @brief Disable further captures when the host closes.
 * @param widget Export control, or NULL.
 * @details Idempotent, main-thread operation. An already accepted immutable write
 * may finish; it cannot access the source widget or an application service.
 */
void UmiTextExportGtk4Retire(GtkWidget *widget);

/**
 * @brief Observe whether an accepted write is awaiting main-thread completion.
 * @param widget Export control, or NULL.
 * @return True until the accepted write completes, including after retirement.
 */
bool UmiTextExportGtk4Busy(GtkWidget *widget);
#ifdef __cplusplus
}
#endif
#endif
