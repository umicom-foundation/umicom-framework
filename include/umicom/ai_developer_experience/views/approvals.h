/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/views/approvals.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral AI Approvals view model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_APPROVALS_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_APPROVALS_H
#include "umicom/ai_developer_experience/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ai_developer_experience/approval_queue.h"
UmiStatus umi_ai_developer_approvals_view_create(
    const char *view_id,
    const UmiAiDeveloperApprovalQueue *approvals,
    const char *active_approval_id,
    size_t visible_rows,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
