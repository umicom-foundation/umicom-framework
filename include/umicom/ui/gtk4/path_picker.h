/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/path_picker.h
 * PURPOSE: Collect a local filename without opening, saving or submitting application work.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_PATH_PICKER_H
#define UMICOM_UI_GTK4_PATH_PICKER_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

/** Choose the native dialog appropriate to the application's next explicit action. */
typedef enum UmiPathPickerMode {
    UMI_PATH_PICKER_EXISTING_FILE = 0,
    UMI_PATH_PICKER_NEW_FILE,
    UMI_PATH_PICKER_DIRECTORY
} UmiPathPickerMode;

/** Seed a native chooser from explicit application context without filesystem
 * access or a current-directory fallback. Call on the GTK owner thread.
 * selectedPath, when supplied, is an absolute local file or directory and wins
 * over fallbackFolder. fallbackFolder is an absolute local directory. Empty
 * strings act as absent inputs. A new-file dialog can also suggest a basename.
 * All supplied values must be UTF-8; relative paths, URIs and path separators
 * in suggestedName are rejected before changing the dialog. With no location,
 * clear prior seeds and let the operating-system chooser select its default.
 * This sets initial navigation only; it never opens or writes the chosen file. */
UmiStatus UmiPathPickerGtk4SetInitialLocation(GtkFileDialog *dialog,
    UmiPathPickerMode mode, const char *selectedPath,
    const char *fallbackFolder, const char *suggestedName);

/**
 * @brief Create a Browse control that fills an existing path entry.
 * @param entry Borrowed entry; only a weak reference is retained.
 * @param mode Select an existing file, a proposed filename, or a directory.
 * @param title UTF-8 dialog title copied by the control.
 * @param suggestedName Optional UTF-8 basename for a new file, copied on creation.
 * @return Floating widget to append beside the entry, or NULL for invalid arguments.
 *
 * Call on the GTK thread. The entry and control must share a visible window.
 * Choosing a path does not read, create, overwrite or submit a file. The host's
 * Open, Export or Build action remains responsible for validation and execution.
 * Closing or hiding the control cancels its pending chooser. A late result is
 * ignored if the entry changed, moved to another window, or was destroyed.
 * The same checks apply during dialog setup: native property observers may
 * close the host or edit its path before dispatch. Such a request is cancelled
 * without starting a chooser. Repeated Browse during setup starts at most one
 * request; completion owns that request once native dispatch begins.
 * Remote URIs and paths exceeding the entry's character limit are rejected
 * without truncating the previous text. Applications may add stricter byte limits.
 */
GtkWidget *UmiPathPickerGtk4Create(GtkEntry *entry, UmiPathPickerMode mode,
    const char *title, const char *suggestedName);
#ifdef __cplusplus
}
#endif
#endif
