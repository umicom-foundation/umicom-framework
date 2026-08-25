/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/call_hierarchy.h
 *
 * PURPOSE:
 *   Represent caller/callee hierarchy edges with stable identity.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_CALL_HIERARCHY_H
#define UMICOM_LANGUAGE_INTELLIGENCE_CALL_HIERARCHY_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_CALL_HIERARCHY_API_VERSION 1U
typedef struct UmiLanguageIntelligenceCallHierarchyEdge {
    uint32_t struct_size;
    uint32_t api_version;
    char source_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char target_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char relation[64];
    uint32_t weight;
    int enabled;
} UmiLanguageIntelligenceCallHierarchyEdge;
void umi_language_intelligence_call_hierarchy_edge_init(UmiLanguageIntelligenceCallHierarchyEdge *edge);
UmiStatus umi_language_intelligence_call_hierarchy_edge_set(
    UmiLanguageIntelligenceCallHierarchyEdge *edge,
    const char *source_id,
    const char *target_id,
    const char *relation,
    uint32_t weight);
UmiStatus umi_language_intelligence_call_hierarchy_edge_validate(const UmiLanguageIntelligenceCallHierarchyEdge *edge);
int umi_language_intelligence_call_hierarchy_edge_matches_source(
    const UmiLanguageIntelligenceCallHierarchyEdge *edge,
    const char *source_id);
#ifdef __cplusplus
}
#endif
#endif
