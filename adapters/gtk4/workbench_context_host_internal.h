/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_context_host_internal.h
 *
 * PURPOSE:
 *   Share private GTK4 adapter helpers for context-host controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_GTK4_INTERNAL_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_GTK4_INTERNAL_H
#include "umicom/workbench_context_host/gtk4.h"
#define UMI_WCH_DATA_HOST "umicom-workbench-context-host"
#define UMI_WCH_DATA_GROUP "umicom-workbench-context-group"
#define UMI_WCH_DATA_ENDPOINT "umicom-workbench-context-endpoint"
#define UMI_WCH_DATA_STATUS "umicom-workbench-context-status"
/**
 * Provide the wch colour css class operation used by this module and its client
 * applications.
 */
const char *umi_wch_colour_css_class(UmiContextChannelColour colour);
/**
 * Provide the wch clear children operation used by this module and its client
 * applications.
 */
void umi_wch_clear_children(GtkWidget *widget);
#endif
