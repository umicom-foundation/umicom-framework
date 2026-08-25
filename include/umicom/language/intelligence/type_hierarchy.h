/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/type_hierarchy.h
 *
 * PURPOSE:
 *   Represent supertype/subtype hierarchy edges with stable identity.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_TYPE_HIERARCHY_H
#define UMICOM_LANGUAGE_INTELLIGENCE_TYPE_HIERARCHY_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_TYPE_HIERARCHY_API_VERSION 1U
typedef struct UmiLanguageIntelligenceTypeHierarchyEdge {
    uint32_t struct_size;
    uint32_t api_version;
    char source_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char target_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char relation[64];
    uint32_t weight;
    int enabled;
} UmiLanguageIntelligenceTypeHierarchyEdge;
void umi_language_intelligence_type_hierarchy_edge_init(UmiLanguageIntelligenceTypeHierarchyEdge *edge);
UmiStatus umi_language_intelligence_type_hierarchy_edge_set(
    UmiLanguageIntelligenceTypeHierarchyEdge *edge,
    const char *source_id,
    const char *target_id,
    const char *relation,
    uint32_t weight);
UmiStatus umi_language_intelligence_type_hierarchy_edge_validate(const UmiLanguageIntelligenceTypeHierarchyEdge *edge);
int umi_language_intelligence_type_hierarchy_edge_matches_source(
    const UmiLanguageIntelligenceTypeHierarchyEdge *edge,
    const char *source_id);
#ifdef __cplusplus
}
#endif
#endif
