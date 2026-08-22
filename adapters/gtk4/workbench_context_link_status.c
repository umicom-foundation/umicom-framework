/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_link_status.c
 *
 * PURPOSE:
 *   Render the reusable GTK4 context-link status surface.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/gtk4.h"

#include <stdio.h>

GtkWidget *umi_workbench_context_link_gtk4_status_new(
    const UmiWorkbenchContextLinkService *service)
{
    char text[256U];
    if (service == NULL) return gtk_label_new("Context links unavailable");
    (void)snprintf(text, sizeof(text),
                   "Groups: %zu  Bindings: %zu  Published: %llu  Delivered: %llu",
                   service->groups.count,
                   service->bindings.count,
                   (unsigned long long)service->metrics.publish_count,
                   (unsigned long long)service->metrics.delivery_count);
    return gtk_label_new(text);
}
