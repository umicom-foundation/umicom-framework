/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/view_ids.h
 *
 * PURPOSE:
 *   Publish stable Framework view identifiers for AI developer panes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_IDS_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_IDS_H
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai developer view id overview operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_view_id_overview(void);
/**
 * Provide the ai developer view id chat operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_view_id_chat(void);
/**
 * Provide the ai developer view id tasks operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_view_id_tasks(void);
/**
 * Provide the ai developer view id approvals operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_view_id_approvals(void);
/**
 * Provide the ai developer view id patch review operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_view_id_patch_review(void);
/**
 * Provide the ai developer view id diff operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_view_id_diff(void);
/**
 * Provide the ai developer view id tool activity operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_view_id_tool_activity(void);
/**
 * Provide the ai developer view id validation operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_view_id_validation(void);
/**
 * Provide the ai developer view id context operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_view_id_context(void);
/**
 * Provide the ai developer view id history operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_view_id_history(void);
/**
 * Provide the ai developer view id policy operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_view_id_policy(void);
/**
 * Provide the ai developer view id checkpoints operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_view_id_checkpoints(void);

#ifdef __cplusplus
}
#endif
#endif
