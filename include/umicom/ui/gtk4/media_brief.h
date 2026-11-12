/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/media_brief.h
 *
 * PURPOSE:
 *   Compose source excerpts, editable briefs and explicit provider handoffs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_MEDIA_BRIEF_H
#define UMICOM_UI_GTK4_MEDIA_BRIEF_H
#include <gtk/gtk.h>
#include <stdbool.h>
#include "umicom/creative_workspace/media_brief.h"
#ifdef __cplusplus
extern "C" {
#endif
/* GTK owner thread. The floating result owns its draft and weakly observes
 * optional generator panels. No storage, network or credential access occurs
 * during creation. A host calls Retire before releasing its service owners;
 * retained controls then become inert. The source excerpt stays in memory
 * until the panel is released. Saving is explicit and uses a chosen path. */
GtkWidget *UmiMediaBriefGtkCreate(GtkWidget *image_panel, GtkWidget *video_panel);
void UmiMediaBriefGtkRetire(GtkWidget *root);
/* Owner-thread observation of a pending creation-document job. Retirement
 * requests cancellation; an atomic save already publishing may still finish. */
bool UmiMediaBriefGtkBusy(GtkWidget *root);
/* Accept a fully validated source draft on the owner thread. Existing source,
 * direction or reviewed prompt is preserved unless replace_existing is true.
 * This invalidates the prior prepared brief; it never starts generation.
 * Returns BUSY while a creation-document save/open owns the panel. */
UmiStatus UmiMediaBriefGtkSetSource(GtkWidget *root, const UmiMediaBriefDraft *draft,
                                  bool replace_existing);
#ifdef __cplusplus
}
#endif
#endif
