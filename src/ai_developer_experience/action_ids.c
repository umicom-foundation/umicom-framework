/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/action_ids.c
 *
 * PURPOSE:
 *   Implement stable AI developer action identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/action_ids.h"

/*
 * Provide the ai developer action open overview operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_open_overview(void)
{ return "ai.developer.open-overview"; }
/*
 * Provide the ai developer action new chat operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_action_new_chat(void)
{ return "ai.developer.new-chat"; }
/*
 * Provide the ai developer action open tasks operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_action_open_tasks(void)
{ return "ai.developer.open-tasks"; }
/*
 * Provide the ai developer action open approvals operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_open_approvals(void)
{ return "ai.developer.open-approvals"; }
/*
 * Provide the ai developer action open review operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_action_open_review(void)
{ return "ai.developer.open-review"; }
/*
 * Provide the ai developer action open diff operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_action_open_diff(void)
{ return "ai.developer.open-diff"; }
/*
 * Provide the ai developer action open tools operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_action_open_tools(void)
{ return "ai.developer.open-tools"; }
/*
 * Provide the ai developer action open validation operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_open_validation(void)
{ return "ai.developer.open-validation"; }
/*
 * Provide the ai developer action open context operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_open_context(void)
{ return "ai.developer.open-context"; }
/*
 * Provide the ai developer action open history operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_open_history(void)
{ return "ai.developer.open-history"; }
/*
 * Provide the ai developer action open policy operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_action_open_policy(void)
{ return "ai.developer.open-policy"; }
/*
 * Provide the ai developer action open checkpoints operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_open_checkpoints(void)
{ return "ai.developer.open-checkpoints"; }
/*
 * Provide the ai developer action approval approve operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_approval_approve(void)
{ return "ai.developer.approval-approve"; }
/*
 * Provide the ai developer action approval reject operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_approval_reject(void)
{ return "ai.developer.approval-reject"; }
/*
 * Perform ai developer action patch through the module contract so client applications do
 * not duplicate its policy.
 */
const char *umi_ai_developer_action_patch_apply(void)
{ return "ai.developer.patch-apply"; }
/*
 * Provide the ai developer action patch reject operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_patch_reject(void)
{ return "ai.developer.patch-reject"; }
/*
 * Provide the ai developer action diff next operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_action_diff_next(void)
{ return "ai.developer.diff-next"; }
/*
 * Provide the ai developer action diff previous operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_diff_previous(void)
{ return "ai.developer.diff-previous"; }
/*
 * Write ai developer action session in its stable representation and report capacity or
 * input failures to the caller.
 */
const char *umi_ai_developer_action_session_save(void)
{ return "ai.developer.session-save"; }
/*
 * Provide the ai developer action session restore operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_session_restore(void)
{ return "ai.developer.session-restore"; }

/*
 * Provide the ai developer action patch select file operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_action_patch_select_file(void)
{ return "ai.developer.patch-select-file"; }
/*
 * Provide the ai developer action patch mark reviewed operation used by this module and
 * its client applications.
 */
const char *umi_ai_developer_action_patch_mark_reviewed(void)
{ return "ai.developer.patch-mark-reviewed"; }
/*
 * Provide the ai developer action patch mark unreviewed operation used by this module and
 * its client applications.
 */
const char *umi_ai_developer_action_patch_mark_unreviewed(void)
{ return "ai.developer.patch-mark-unreviewed"; }
