/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/progress_tracking.c
 *
 * PURPOSE:
 *   Implement track bounded work progress and terminal state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/progress_tracking.h"
#include <string.h>
/*
 * Initialise language intelligence progress tracking from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_progress_tracking_init(UmiLanguageIntelligenceProgressTracking *progress, const char *id, uint64_t total)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (progress == NULL) return;
    (void)memset(progress, 0, sizeof(*progress));
    progress->struct_size = (uint32_t)sizeof(*progress);
    progress->api_version = UMI_LANGUAGE_INTELLIGENCE_PROGRESS_TRACKING_API_VERSION;
    progress->total = total;
    progress->state = UMI_LANGUAGE_INTELLIGENCE_STARTING;
    progress->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_language_intelligence_copy_text(progress->id, sizeof(progress->id), id);
}
/*
 * Provide the language intelligence progress tracking advance operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_progress_tracking_advance(UmiLanguageIntelligenceProgressTracking *progress, uint64_t amount)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (progress == NULL || progress->total == 0U || progress->id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (amount > progress->total - progress->completed)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    progress->completed += amount;
    progress->state = progress->completed == progress->total
        ? UMI_LANGUAGE_INTELLIGENCE_READY
        : UMI_LANGUAGE_INTELLIGENCE_DEGRADED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (progress->revision != UINT64_MAX) progress->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the language intelligence progress tracking basis points operation used by this
 * module and its client applications.
 */
uint32_t umi_language_intelligence_progress_tracking_basis_points(const UmiLanguageIntelligenceProgressTracking *progress)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (progress == NULL || progress->total == 0U) return 0U;
    return (uint32_t)((progress->completed * UINT64_C(10000)) / progress->total);
}
/*
 * Provide the language intelligence progress tracking finish operation used by this module
 * and its client applications.
 */
void umi_language_intelligence_progress_tracking_finish(UmiLanguageIntelligenceProgressTracking *progress)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (progress == NULL) return;
    progress->completed = progress->total;
    progress->state = UMI_LANGUAGE_INTELLIGENCE_READY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (progress->revision != UINT64_MAX) progress->revision += 1U;
}
