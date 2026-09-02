/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/action_ids.h
 *
 * PURPOSE:
 *   Publish stable Framework action identifiers used by toolkit-neutral AI
 *   developer views and host command registries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_ACTION_IDS_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_ACTION_IDS_H
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai developer action open overview operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_open_overview(void);
/**
 * Provide the ai developer action new chat operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_action_new_chat(void);
/**
 * Provide the ai developer action open tasks operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_action_open_tasks(void);
/**
 * Provide the ai developer action open approvals operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_open_approvals(void);
/**
 * Provide the ai developer action open review operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_action_open_review(void);
/**
 * Provide the ai developer action open diff operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_action_open_diff(void);
/**
 * Provide the ai developer action open tools operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_action_open_tools(void);
/**
 * Provide the ai developer action open validation operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_open_validation(void);
/**
 * Provide the ai developer action open context operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_open_context(void);
/**
 * Provide the ai developer action open history operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_open_history(void);
/**
 * Provide the ai developer action open policy operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_action_open_policy(void);
/**
 * Provide the ai developer action open checkpoints operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_open_checkpoints(void);
/**
 * Provide the ai developer action approval approve operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_approval_approve(void);
/**
 * Provide the ai developer action approval reject operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_approval_reject(void);
/**
 * Perform ai developer action patch through the module contract so client applications do
 * not duplicate its policy.
 */
const char *umi_ai_developer_action_patch_apply(void);
/**
 * Provide the ai developer action patch reject operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_patch_reject(void);
/**
 * Provide the ai developer action patch select file operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_patch_select_file(void);
/**
 * Provide the ai developer action patch mark reviewed operation used by this module and
 * its client applications.
 */
const char *umi_ai_developer_action_patch_mark_reviewed(void);
/**
 * Provide the ai developer action patch mark unreviewed operation used by this module and
 * its client applications.
 */
const char *umi_ai_developer_action_patch_mark_unreviewed(void);
/**
 * Provide the ai developer action diff next operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_action_diff_next(void);
/**
 * Provide the ai developer action diff previous operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_diff_previous(void);
/**
 * Write ai developer action session in its stable representation and report capacity or
 * input failures to the caller.
 */
const char *umi_ai_developer_action_session_save(void);
/**
 * Provide the ai developer action session restore operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_session_restore(void);

#ifdef __cplusplus
}
#endif
#endif
