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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_CALL_HIERARCHY_H
#define UMICOM_LANGUAGE_INTELLIGENCE_CALL_HIERARCHY_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_CALL_HIERARCHY_API_VERSION 1U
/**
 * Represent the language intelligence call hierarchy edge data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceCallHierarchyEdge {
    uint32_t struct_size;
    uint32_t api_version;
    char source_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char target_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char relation[64];
    uint32_t weight;
    int enabled;
} UmiLanguageIntelligenceCallHierarchyEdge;
/**
 * Initialise language intelligence call hierarchy edge from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_call_hierarchy_edge_init(UmiLanguageIntelligenceCallHierarchyEdge *edge);
/**
 * Copy language intelligence call hierarchy edge into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_language_intelligence_call_hierarchy_edge_set(
    UmiLanguageIntelligenceCallHierarchyEdge *edge,
    const char *source_id,
    const char *target_id,
    const char *relation,
    uint32_t weight);
/**
 * Check that language intelligence call hierarchy edge satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_language_intelligence_call_hierarchy_edge_validate(const UmiLanguageIntelligenceCallHierarchyEdge *edge);
/**
 * Provide the language intelligence call hierarchy edge matches source operation used by
 * this module and its client applications.
 */
int umi_language_intelligence_call_hierarchy_edge_matches_source(
    const UmiLanguageIntelligenceCallHierarchyEdge *edge,
    const char *source_id);
#ifdef __cplusplus
}
#endif
#endif
