/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/automation_pointer_gtk4.c
 *
 * PURPOSE:
 *   Share centre-point GTK hit testing with UAT callers without interpreting
 *   successful callback activation as proof of pointer accessibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation_pointer.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/interaction_recording.h"
#include <gtk/gtk.h>
#include <math.h>
#include <string.h>

/* Copy only type metadata. Never collect captions, entry values or the ID of
 * an unrelated widget which happens to intercept the chosen point. */
UmiStatus UmiGtk4AutomationProbePointer(void *nativeScope, const char *targetId,
    UmiGtk4AutomationPointerSample *outSample)
{
    void *resolved = NULL;
    GtkWidget *target;
    GtkWidget *window;
    GtkWidget *hit;
    GtkRoot *root;
    graphene_rect_t bounds;
    UmiStatus status;
    if (outSample == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(outSample, 0, sizeof(*outSample));
    status = umi_gtk4_automation_find_tagged_widget_checked(nativeScope, targetId, &resolved);
    if (status != UMI_STATUS_OK) return status;
    target = GTK_WIDGET(resolved);
    if (UmiGtk4RecordingIsPrivate(target)) return UMI_STATUS_PERMISSION_DENIED;
    g_strlcpy(outSample->targetType, G_OBJECT_TYPE_NAME(target), sizeof(outSample->targetType));
    outSample->state = UMI_GTK4_POINTER_UNMAPPED;
    root = gtk_widget_get_root(target);
    if (root == NULL || !GTK_IS_WINDOW(root) || !gtk_widget_is_visible(target) ||
        !gtk_widget_get_mapped(target)) return UMI_STATUS_OK;
    window = GTK_WIDGET(root);
    if (!gtk_widget_is_sensitive(target)) {
        outSample->state = UMI_GTK4_POINTER_DISABLED;
        return UMI_STATUS_OK;
    }
    outSample->state = UMI_GTK4_POINTER_OUTSIDE;
    if (!gtk_widget_compute_bounds(target, window, &bounds) ||
        !isfinite(bounds.origin.x) || !isfinite(bounds.origin.y) ||
        !isfinite(bounds.size.width) || !isfinite(bounds.size.height) ||
        bounds.size.width <= 0.0F || bounds.size.height <= 0.0F) return UMI_STATUS_OK;
    outSample->windowX = bounds.origin.x + bounds.size.width / 2.0F;
    outSample->windowY = bounds.origin.y + bounds.size.height / 2.0F;
    if (!isfinite(outSample->windowX) || !isfinite(outSample->windowY) ||
        outSample->windowX < 0.0F || outSample->windowY < 0.0F ||
        outSample->windowX >= (float)gtk_widget_get_width(window) ||
        outSample->windowY >= (float)gtk_widget_get_height(window)) return UMI_STATUS_OK;
    /* Pick from the whole window, not the lookup subtree: a sibling overlay
     * can obstruct the target even when it lies outside the recorded scope. */
    hit = gtk_widget_pick(window, outSample->windowX, outSample->windowY, GTK_PICK_DEFAULT);
    if (hit != NULL && !UmiGtk4RecordingIsPrivate(hit))
        g_strlcpy(outSample->hitType, G_OBJECT_TYPE_NAME(hit), sizeof(outSample->hitType));
    outSample->state = hit == target || (hit != NULL && gtk_widget_is_ancestor(hit, target))
        ? UMI_GTK4_POINTER_READY : UMI_GTK4_POINTER_OBSTRUCTED;
    return UMI_STATUS_OK;
}
