/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_host_internal.c
 *
 * PURPOSE:
 *   Implement private GTK4 child-management and semantic colour CSS helpers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "workbench_context_host_internal.h"
const char *umi_wch_colour_css_class(UmiContextChannelColour colour)
{
    switch(colour){
    case UMI_CONTEXT_COLOUR_RED:return "umicom-context-group-red";
    case UMI_CONTEXT_COLOUR_ORANGE:return "umicom-context-group-orange";
    case UMI_CONTEXT_COLOUR_YELLOW:return "umicom-context-group-yellow";
    case UMI_CONTEXT_COLOUR_GREEN:return "umicom-context-group-green";
    case UMI_CONTEXT_COLOUR_CYAN:return "umicom-context-group-cyan";
    case UMI_CONTEXT_COLOUR_BLUE:return "umicom-context-group-blue";
    case UMI_CONTEXT_COLOUR_PURPLE:return "umicom-context-group-purple";
    case UMI_CONTEXT_COLOUR_MAGENTA:return "umicom-context-group-magenta";
    default:return "umicom-context-group-none";
    }
}
void umi_wch_clear_children(GtkWidget *widget)
{
    GtkWidget *child;if(!GTK_IS_BOX(widget))return;
    child=gtk_widget_get_first_child(widget);
    while(child){GtkWidget *next=gtk_widget_get_next_sibling(child);gtk_box_remove(GTK_BOX(widget),child);child=next;}
}
