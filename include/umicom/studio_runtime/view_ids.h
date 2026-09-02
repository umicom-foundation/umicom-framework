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
/**
 * Provide the studio view id runtime overview operation used by this module and its client
 * applications.
 */
const char *umi_studio_view_id_runtime_overview(void);
/**
 * Provide the studio view id document tabs operation used by this module and its client
 * applications.
 */
const char *umi_studio_view_id_document_tabs(void);
/**
 * Provide the studio view id status operation used by this module and its client
 * applications.
 */
const char *umi_studio_view_id_status(void);
/**
 * Provide the studio view id command search operation used by this module and its client
 * applications.
 */
const char *umi_studio_view_id_command_search(void);
/**
 * Provide the studio view id layout browser operation used by this module and its client
 * applications.
 */
const char *umi_studio_view_id_layout_browser(void);
/**
 * Provide the studio view id close guard operation used by this module and its client
 * applications.
 */
const char *umi_studio_view_id_close_guard(void);
/**
 * Provide the studio view id contract operation used by this module and its client
 * applications.
 */
const char *umi_studio_view_id_contract(void);
/**
 * Provide the studio view id selection operation used by this module and its client
 * applications.
 */
const char *umi_studio_view_id_selection(void);
/**
 * Provide the studio view id shell state operation used by this module and its client
 * applications.
 */
const char *umi_studio_view_id_shell_state(void);
#ifdef __cplusplus
}
#endif
#endif
