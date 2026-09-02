/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/view_helpers.h
 *
 * PURPOSE:
 *   Provide shared toolkit-neutral property/action helpers for all AI developer
 *   experience view factories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_HELPERS_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_HELPERS_H

#include "umicom/ai_developer_experience/types.h"
#include "umicom/ui/command_view.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai developer view create base operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_view_create_base(
    const char *view_id,
    const char *kind,
    const char *title,
    const char *summary,
    UmiUiViewModel **out_view);

/**
 * Provide the ai developer view set string operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_view_set_string(
    UmiUiViewModel *view,
    const char *key,
    const char *text);

/**
 * Provide the ai developer view set integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_view_set_integer(
    UmiUiViewModel *view,
    const char *key,
    int64_t value);

/**
 * Provide the ai developer view set boolean operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_view_set_boolean(
    UmiUiViewModel *view,
    const char *key,
    int value);

/**
 * Provide the ai developer view set action operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_view_set_action(
    UmiUiViewModel *view,
    size_t index,
    const char *action_id,
    const char *label,
    const char *tooltip,
    int enabled);

#ifdef __cplusplus
}
#endif
#endif
