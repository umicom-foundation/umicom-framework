/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/navigation_graph.c
 *
 * PURPOSE:
 *   Implement represent typed navigation edges between symbols/documents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/navigation_graph.h"
#include <string.h>
/*
 * Initialise language intelligence navigation graph edge from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_navigation_graph_edge_init(UmiLanguageIntelligenceNavigationGraphEdge *edge)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (edge == NULL) return;
    (void)memset(edge, 0, sizeof(*edge));
    edge->struct_size = (uint32_t)sizeof(*edge);
    edge->api_version = UMI_LANGUAGE_INTELLIGENCE_NAVIGATION_GRAPH_API_VERSION;
    edge->enabled = 1;
}
/*
 * Copy language intelligence navigation graph edge into module-owned storage so callers
 * keep ownership of their input values.
 */
UmiStatus umi_language_intelligence_navigation_graph_edge_set(
    UmiLanguageIntelligenceNavigationGraphEdge *edge,
    const char *source_id,
    const char *target_id,
    const char *relation,
    uint32_t weight)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (edge == NULL || source_id == NULL || target_id == NULL || relation == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_language_intelligence_copy_text(edge->source_id, sizeof(edge->source_id), source_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_language_intelligence_copy_text(edge->target_id, sizeof(edge->target_id), target_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_language_intelligence_copy_text(edge->relation, sizeof(edge->relation), relation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    edge->weight = weight;
    return UMI_STATUS_OK;
}
/*
 * Check that language intelligence navigation graph edge satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_language_intelligence_navigation_graph_edge_validate(const UmiLanguageIntelligenceNavigationGraphEdge *edge)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (edge == NULL || edge->source_id[0] == '\0' ||
        edge->target_id[0] == '\0' || edge->relation[0] == '\0' ||
        edge->weight == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Provide the language intelligence navigation graph edge matches source operation used by
 * this module and its client applications.
 */
int umi_language_intelligence_navigation_graph_edge_matches_source(
    const UmiLanguageIntelligenceNavigationGraphEdge *edge,
    const char *source_id)
{
    return umi_language_intelligence_navigation_graph_edge_validate(edge) == UMI_STATUS_OK &&
        edge->enabled != 0 && source_id != NULL &&
        strcmp(edge->source_id, source_id) == 0;
}
