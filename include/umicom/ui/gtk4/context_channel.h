/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/context_channel.h
 *
 * PURPOSE:
 *   Declare GTK4 factories for context-channel and cross-application panel surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_CONTEXT_CHANNEL_H
#define UMICOM_UI_GTK4_CONTEXT_CHANNEL_H
#ifdef __cplusplus
extern "C" {
#endif
void *umi_gtk4_context_channel_badge_new(const char *title,const char *detail);
void *umi_gtk4_context_channel_menu_new(const char *title,const char *detail);
void *umi_gtk4_context_channel_inspector_new(const char *title,const char *detail);
void *umi_gtk4_context_channel_history_new(const char *title,const char *detail);
void *umi_gtk4_context_channel_diagnostics_new(const char *title,const char *detail);
void *umi_gtk4_context_channel_status_new(const char *title,const char *detail);
void *umi_gtk4_cross_application_panel_header_new(const char *title,const char *detail);
void *umi_gtk4_cross_application_panel_host_new(const char *title,const char *detail);
void *umi_gtk4_context_channel_group_picker_new(const char *title,const char *detail);
void *umi_gtk4_context_channel_toolbar_new(const char *title,const char *detail);
#ifdef __cplusplus
}
#endif
#endif
