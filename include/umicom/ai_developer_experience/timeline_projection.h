/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/timeline_projection.h
 *
 * PURPOSE:
 *   Project existing agent events, tool results and validation evidence into one
 *   consistent AI developer timeline.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_TIMELINE_PROJECTION_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_TIMELINE_PROJECTION_H

#include "umicom/ai_developer_experience/timeline.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai developer timeline from agent event operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_timeline_from_agent_event(
    const UmiAiCodingEvent *event,
    UmiAiDeveloperTimelineEvent *out_event);

/**
 * Provide the ai developer timeline from tool result operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_timeline_from_tool_result(
    const char *task_id,
    const UmiAiCodingToolResult *result,
    UmiAiDeveloperTimelineEvent *out_event);

/**
 * Provide the ai developer timeline from validation operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_timeline_from_validation(
    const char *task_id,
    const UmiAiCodingValidationResult *result,
    uint64_t sequence,
    UmiAiDeveloperTimelineEvent *out_event);

/**
 * Provide the ai developer timeline from approval operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_timeline_from_approval(
    const UmiAiDeveloperApprovalRequest *approval,
    UmiAiDeveloperTimelineEvent *out_event);

#ifdef __cplusplus
}
#endif
#endif
