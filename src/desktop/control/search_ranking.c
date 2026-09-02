/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/search_ranking.c
 * PURPOSE: Implement store search-ranking policy weights shared by global search consumers.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/search_ranking.h"

#include <string.h>
#include <ctype.h>

/*
 * Initialise desktop search ranking from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_search_ranking_init(UmiDesktopSearchRankingSnapshot *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_SEARCH_RANKING_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that desktop search ranking satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_desktop_search_ranking_validate(const UmiDesktopSearchRankingSnapshot *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_SEARCH_RANKING_API_VERSION ||
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
 * Provide the desktop search ranking set subject operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_search_ranking_set_subject(UmiDesktopSearchRankingSnapshot *value, const char *subject_id)
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
 * Provide the desktop search ranking set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_search_ranking_set_detail(UmiDesktopSearchRankingSnapshot *value, const char *detail)
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
 * Provide the desktop search ranking set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_search_ranking_set_state(UmiDesktopSearchRankingSnapshot *value, uint32_t state)
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
 * Provide the desktop search ranking set priority operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_search_ranking_set_priority(UmiDesktopSearchRankingSnapshot *value, uint32_t priority)
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
 * Provide the desktop search ranking set enabled operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_search_ranking_set_enabled(UmiDesktopSearchRankingSnapshot *value, bool enabled)
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
 * Provide the desktop search ranking same identity operation used by this module and its
 * client applications.
 */
bool umi_desktop_search_ranking_same_identity(const UmiDesktopSearchRankingSnapshot *left, const UmiDesktopSearchRankingSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

/*
 * Provide the desktop search ranking score operation used by this module and its client
 * applications.
 */
uint32_t umi_desktop_search_ranking_score(const char *query, const char *label, uint32_t base_priority)
{
    uint32_t score_value = base_priority;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || label == NULL) return 0U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_desktop_control_text_contains(label, query)) score_value += 1000U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (query[0] != '\0' && label[0] != '\0' &&
        tolower((int)(unsigned char)query[0]) == tolower((int)(unsigned char)label[0])) {
        score_value += 100U;
    }
    return score_value;
}
