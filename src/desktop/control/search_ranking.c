/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/search_ranking.c
 * PURPOSE: Implement store search-ranking policy weights shared by global search consumers.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/search_ranking.h"

#include <string.h>
#include <ctype.h>

void umi_desktop_search_ranking_init(UmiDesktopSearchRankingSnapshot *value, const char *id)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_SEARCH_RANKING_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

UmiStatus umi_desktop_search_ranking_validate(const UmiDesktopSearchRankingSnapshot *value)
{
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_SEARCH_RANKING_API_VERSION ||
        !umi_desktop_control_id_valid(value->id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (value->subject_id[0] != '\0' && !umi_desktop_control_id_valid(value->subject_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_search_ranking_set_subject(UmiDesktopSearchRankingSnapshot *value, const char *subject_id)
{
    UmiStatus status;
    if (value == NULL || subject_id == NULL || !umi_desktop_control_id_valid(subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->subject_id, sizeof(value->subject_id), subject_id);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_search_ranking_set_detail(UmiDesktopSearchRankingSnapshot *value, const char *detail)
{
    UmiStatus status;
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->detail, sizeof(value->detail), detail);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_search_ranking_set_state(UmiDesktopSearchRankingSnapshot *value, uint32_t state)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_search_ranking_set_priority(UmiDesktopSearchRankingSnapshot *value, uint32_t priority)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->priority = priority;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_search_ranking_set_enabled(UmiDesktopSearchRankingSnapshot *value, bool enabled)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->enabled = enabled;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

bool umi_desktop_search_ranking_same_identity(const UmiDesktopSearchRankingSnapshot *left, const UmiDesktopSearchRankingSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

uint32_t umi_desktop_search_ranking_score(const char *query, const char *label, uint32_t base_priority)
{
    uint32_t score_value = base_priority;
    if (query == NULL || label == NULL) return 0U;
    if (umi_desktop_control_text_contains(label, query)) score_value += 1000U;
    if (query[0] != '\0' && label[0] != '\0' &&
        tolower((int)(unsigned char)query[0]) == tolower((int)(unsigned char)label[0])) {
        score_value += 100U;
    }
    return score_value;
}
