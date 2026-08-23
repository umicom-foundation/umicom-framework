/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/views/policy.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral AI Tool Policy view model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_POLICY_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_POLICY_H
#include "umicom/ai_developer_experience/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ai_coding_tools/environment.h"
UmiStatus umi_ai_developer_policy_view_create(
    const char *view_id,
    const UmiAiCodingToolEnvironment *environment,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
