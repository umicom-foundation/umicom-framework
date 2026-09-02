/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/navigation_graph.h
 *
 * PURPOSE:
 *   Represent typed navigation edges between symbols/documents.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_NAVIGATION_GRAPH_H
#define UMICOM_LANGUAGE_INTELLIGENCE_NAVIGATION_GRAPH_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_NAVIGATION_GRAPH_API_VERSION 1U
/**
 * Represent the language intelligence navigation graph edge data shared with callers of
 * this public contract.
 */
typedef struct UmiLanguageIntelligenceNavigationGraphEdge {
    uint32_t struct_size;
    uint32_t api_version;
    char source_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char target_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char relation[64];
    uint32_t weight;
    int enabled;
} UmiLanguageIntelligenceNavigationGraphEdge;
/**
 * Initialise language intelligence navigation graph edge from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_navigation_graph_edge_init(UmiLanguageIntelligenceNavigationGraphEdge *edge);
/**
 * Copy language intelligence navigation graph edge into module-owned storage so callers
 * keep ownership of their input values.
 */
UmiStatus umi_language_intelligence_navigation_graph_edge_set(
    UmiLanguageIntelligenceNavigationGraphEdge *edge,
    const char *source_id,
    const char *target_id,
    const char *relation,
    uint32_t weight);
/**
 * Check that language intelligence navigation graph edge satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_language_intelligence_navigation_graph_edge_validate(const UmiLanguageIntelligenceNavigationGraphEdge *edge);
/**
 * Provide the language intelligence navigation graph edge matches source operation used by
 * this module and its client applications.
 */
int umi_language_intelligence_navigation_graph_edge_matches_source(
    const UmiLanguageIntelligenceNavigationGraphEdge *edge,
    const char *source_id);
#ifdef __cplusplus
}
#endif
#endif
