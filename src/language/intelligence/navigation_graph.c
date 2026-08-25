/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/navigation_graph.c
 *
 * PURPOSE:
 *   Implement represent typed navigation edges between symbols/documents.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/navigation_graph.h"
#include <string.h>
void umi_language_intelligence_navigation_graph_edge_init(UmiLanguageIntelligenceNavigationGraphEdge *edge)
{
    if (edge == NULL) return;
    (void)memset(edge, 0, sizeof(*edge));
    edge->struct_size = (uint32_t)sizeof(*edge);
    edge->api_version = UMI_LANGUAGE_INTELLIGENCE_NAVIGATION_GRAPH_API_VERSION;
    edge->enabled = 1;
}
UmiStatus umi_language_intelligence_navigation_graph_edge_set(
    UmiLanguageIntelligenceNavigationGraphEdge *edge,
    const char *source_id,
    const char *target_id,
    const char *relation,
    uint32_t weight)
{
    UmiStatus status;
    if (edge == NULL || source_id == NULL || target_id == NULL || relation == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_language_intelligence_copy_text(edge->source_id, sizeof(edge->source_id), source_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_language_intelligence_copy_text(edge->target_id, sizeof(edge->target_id), target_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_language_intelligence_copy_text(edge->relation, sizeof(edge->relation), relation);
    if (status != UMI_STATUS_OK) return status;
    edge->weight = weight;
    return UMI_STATUS_OK;
}
UmiStatus umi_language_intelligence_navigation_graph_edge_validate(const UmiLanguageIntelligenceNavigationGraphEdge *edge)
{
    if (edge == NULL || edge->source_id[0] == '\0' ||
        edge->target_id[0] == '\0' || edge->relation[0] == '\0' ||
        edge->weight == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
int umi_language_intelligence_navigation_graph_edge_matches_source(
    const UmiLanguageIntelligenceNavigationGraphEdge *edge,
    const char *source_id)
{
    return umi_language_intelligence_navigation_graph_edge_validate(edge) == UMI_STATUS_OK &&
        edge->enabled != 0 && source_id != NULL &&
        strcmp(edge->source_id, source_id) == 0;
}
