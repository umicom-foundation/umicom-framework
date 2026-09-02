/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/view_ids.h
 *
 * PURPOSE:
 *   Stable toolkit-neutral IDs for IDE integration status/control panes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_VIEW_IDS_H
#define UMICOM_IDE_INTEGRATION_VIEW_IDS_H
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ide view id overview operation used by this module and its client
 * applications.
 */
const char *umi_ide_view_id_overview(void);
/**
 * Provide the ide view id active context operation used by this module and its client
 * applications.
 */
const char *umi_ide_view_id_active_context(void);
/**
 * Provide the ide view id workflow operation used by this module and its client
 * applications.
 */
const char *umi_ide_view_id_workflow(void);
/**
 * Provide the ide view id inline ai operation used by this module and its client
 * applications.
 */
const char *umi_ide_view_id_inline_ai(void);
/**
 * Provide the ide view id navigation operation used by this module and its client
 * applications.
 */
const char *umi_ide_view_id_navigation(void);
/**
 * Provide the ide view id surfaces operation used by this module and its client
 * applications.
 */
const char *umi_ide_view_id_surfaces(void);
/**
 * Provide the ide view id self host operation used by this module and its client
 * applications.
 */
const char *umi_ide_view_id_self_host(void);
/**
 * Provide the ide view id cross domain operation used by this module and its client
 * applications.
 */
const char *umi_ide_view_id_cross_domain(void);
#ifdef __cplusplus
}
#endif
#endif
