/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/view_ids.h
 *
 * PURPOSE:
 *   Publish stable IDs for Framework-owned Studio runtime diagnostic/control
 *   views used by GTK4 and future frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_VIEW_IDS_H
#define UMICOM_STUDIO_RUNTIME_VIEW_IDS_H
#ifdef __cplusplus
extern "C" {
#endif
const char *umi_studio_view_id_runtime_overview(void);
const char *umi_studio_view_id_document_tabs(void);
const char *umi_studio_view_id_status(void);
const char *umi_studio_view_id_command_search(void);
const char *umi_studio_view_id_layout_browser(void);
const char *umi_studio_view_id_close_guard(void);
const char *umi_studio_view_id_contract(void);
const char *umi_studio_view_id_selection(void);
const char *umi_studio_view_id_shell_state(void);
#ifdef __cplusplus
}
#endif
#endif
