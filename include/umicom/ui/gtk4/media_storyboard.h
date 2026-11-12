/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/media_storyboard.h
 *
 * PURPOSE:
 *   Compose native scene authoring with existing asset-library and creation-brief owners.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_MEDIA_STORYBOARD_H
#define UMICOM_UI_GTK4_MEDIA_STORYBOARD_H
#include <gtk/gtk.h>
#include <stdbool.h>
/* Owner thread, floating result. Optional targets are weakly observed and not
 * reparented. Construction performs no I/O. Retire before destroying host services.
 * The original session-only contract is retained below as history; exported
 * instructions remain text, not an editable project archive. */
/* Earlier limitation: Scenes are session drafts; exported instructions are text,
 * not a project archive. The scene-document disclosure now uses Framework
 * versioned save/open services and explicit replacement decisions. It preserves
 * applied scenes only; pending form edits must be applied or discarded first. */
#ifdef __cplusplus
extern "C" {
#endif
GtkWidget *UmiMediaStoryboardGtkCreate(GtkWidget *asset_library, GtkWidget *creation_brief);
void UmiMediaStoryboardGtkRetire(GtkWidget *root);
/* Owner-thread observation of the one pending file job. Retirement requests
 * cancellation; a save already publishing may still complete. */
bool UmiMediaStoryboardGtkBusy(GtkWidget *root);
#ifdef __cplusplus
}
#endif
#endif
