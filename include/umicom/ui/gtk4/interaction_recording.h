/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/interaction_recording.h
 *
 * PURPOSE:
 *   Offer opt-in GUI recording and application-only PNG capture through GTK4.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_INTERACTION_RECORDING_H
#define UMICOM_UI_GTK4_INTERACTION_RECORDING_H
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Show or focus the recorder for one native GtkWindow. The window is weakly
 * observed. Recording is OFF until Start is chosen; closing the panel stops it.
 * Events are restricted to this window and its GTK transient dialogs. Native
 * OS file choosers and other applications are outside that scope. Inputs are
 * logged without character values, clipboard contents or widget text.
 * Explicit notes and screenshots may contain private data: users review the
 * local export before sharing it. See docs/GUI_RECORDING.html for a walkthrough.
 * All GTK functions in this header require the GTK owning thread.
 */
UmiStatus UmiGtk4RecordingPanelShow(void *nativeWindow, const char *applicationId);
/** Same panel with a caller-selected absolute base directory. A unique child
 * directory is created; existing files are never replaced. Test hosts use a
 * temporary directory to keep fixture exports out of real user profiles.
 * Pass NULL for the normal user-data directory. */
UmiStatus UmiGtk4RecordingPanelShowAt(void *nativeWindow, const char *applicationId,
    const char *reportDirectory);
/** Capture one mapped widget to a NEW PNG path. Existing files are rejected.
 * This renders GTK content, not the desktop, cursor, video or foreign windows.
 * Built-in password fields and marked private subtrees block the entire image.
 * The caller must obtain screenshot consent. Other visible text is included.
 * The image is bounded to 4096 x 4096 pixels and 16 MiB encoded PNG.
 */
UmiStatus UmiGtk4RecordingCapturePng(void *nativeWidget, const char *path);
/** Mark a custom control/subtree as private. It blocks screenshots of any
 * containing widget and suppresses its identifiers in the interaction log.
 * This supplements built-in password detection; it is not OCR redaction.
 */
UmiStatus UmiGtk4RecordingSetPrivate(void *nativeWidget, int isPrivate);
/** Test/query whether a visible private control would block an image. */
int UmiGtk4RecordingContainsPrivate(void *nativeWidget);
#ifdef __cplusplus
}
#endif
#endif
