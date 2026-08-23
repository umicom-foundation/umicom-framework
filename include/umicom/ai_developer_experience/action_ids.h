/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/action_ids.h
 *
 * PURPOSE:
 *   Publish stable Framework action identifiers used by toolkit-neutral AI
 *   developer views and host command registries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_ACTION_IDS_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_ACTION_IDS_H
#ifdef __cplusplus
extern "C" {
#endif

const char *umi_ai_developer_action_open_overview(void);
const char *umi_ai_developer_action_new_chat(void);
const char *umi_ai_developer_action_open_tasks(void);
const char *umi_ai_developer_action_open_approvals(void);
const char *umi_ai_developer_action_open_review(void);
const char *umi_ai_developer_action_open_diff(void);
const char *umi_ai_developer_action_open_tools(void);
const char *umi_ai_developer_action_open_validation(void);
const char *umi_ai_developer_action_open_context(void);
const char *umi_ai_developer_action_open_history(void);
const char *umi_ai_developer_action_open_policy(void);
const char *umi_ai_developer_action_open_checkpoints(void);
const char *umi_ai_developer_action_approval_approve(void);
const char *umi_ai_developer_action_approval_reject(void);
const char *umi_ai_developer_action_patch_apply(void);
const char *umi_ai_developer_action_patch_reject(void);
const char *umi_ai_developer_action_patch_select_file(void);
const char *umi_ai_developer_action_patch_mark_reviewed(void);
const char *umi_ai_developer_action_patch_mark_unreviewed(void);
const char *umi_ai_developer_action_diff_next(void);
const char *umi_ai_developer_action_diff_previous(void);
const char *umi_ai_developer_action_session_save(void);
const char *umi_ai_developer_action_session_restore(void);

#ifdef __cplusplus
}
#endif
#endif
