/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/command_ranking.c
 * PURPOSE: Implement store global command ranking policy independent from a concrete palette widget.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/command_ranking.h"

#include <string.h>

/*
 * Initialise desktop command ranking from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_command_ranking_init(UmiDesktopCommandRankingSnapshot *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_COMMAND_RANKING_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that desktop command ranking satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_desktop_command_ranking_validate(const UmiDesktopCommandRankingSnapshot *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_COMMAND_RANKING_API_VERSION ||
        !umi_desktop_control_id_valid(value->id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (value->subject_id[0] != '\0' && !umi_desktop_control_id_valid(value->subject_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop command ranking set subject operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_command_ranking_set_subject(UmiDesktopCommandRankingSnapshot *value, const char *subject_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || subject_id == NULL || !umi_desktop_control_id_valid(subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->subject_id, sizeof(value->subject_id), subject_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

/*
 * Provide the desktop command ranking set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_command_ranking_set_detail(UmiDesktopCommandRankingSnapshot *value, const char *detail)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->detail, sizeof(value->detail), detail);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

/*
 * Provide the desktop command ranking set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_command_ranking_set_state(UmiDesktopCommandRankingSnapshot *value, uint32_t state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop command ranking set priority operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_command_ranking_set_priority(UmiDesktopCommandRankingSnapshot *value, uint32_t priority)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->priority = priority;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop command ranking set enabled operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_command_ranking_set_enabled(UmiDesktopCommandRankingSnapshot *value, bool enabled)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->enabled = enabled;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop command ranking same identity operation used by this module and its
 * client applications.
 */
bool umi_desktop_command_ranking_same_identity(const UmiDesktopCommandRankingSnapshot *left, const UmiDesktopCommandRankingSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

/*
 * Provide the desktop command ranking score operation used by this module and its client
 * applications.
 */
uint32_t umi_desktop_command_ranking_score(uint32_t base_priority, uint32_t invocation_count, bool context_match)
{
    uint64_t score_value = (uint64_t)base_priority + (uint64_t)invocation_count;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (context_match) score_value += UINT64_C(1000);
    return score_value > UINT32_MAX ? UINT32_MAX : (uint32_t)score_value;
}
