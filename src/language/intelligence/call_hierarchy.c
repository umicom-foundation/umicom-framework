/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/call_hierarchy.c
 *
 * PURPOSE:
 *   Implement represent caller/callee hierarchy edges with stable identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/call_hierarchy.h"
#include <string.h>
void umi_language_intelligence_call_hierarchy_edge_init(UmiLanguageIntelligenceCallHierarchyEdge *edge)
{
    if (edge == NULL) return;
    (void)memset(edge, 0, sizeof(*edge));
    edge->struct_size = (uint32_t)sizeof(*edge);
    edge->api_version = UMI_LANGUAGE_INTELLIGENCE_CALL_HIERARCHY_API_VERSION;
    edge->enabled = 1;
}
UmiStatus umi_language_intelligence_call_hierarchy_edge_set(
    UmiLanguageIntelligenceCallHierarchyEdge *edge,
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
UmiStatus umi_language_intelligence_call_hierarchy_edge_validate(const UmiLanguageIntelligenceCallHierarchyEdge *edge)
{
    if (edge == NULL || edge->source_id[0] == '\0' ||
        edge->target_id[0] == '\0' || edge->relation[0] == '\0' ||
        edge->weight == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
int umi_language_intelligence_call_hierarchy_edge_matches_source(
    const UmiLanguageIntelligenceCallHierarchyEdge *edge,
    const char *source_id)
{
    return umi_language_intelligence_call_hierarchy_edge_validate(edge) == UMI_STATUS_OK &&
        edge->enabled != 0 && source_id != NULL &&
        strcmp(edge->source_id, source_id) == 0;
}
