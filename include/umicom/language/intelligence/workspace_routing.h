/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/workspace_routing.h
 *
 * PURPOSE:
 *   Route workspace requests to the appropriate language-service provider.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_WORKSPACE_ROUTING_H
#define UMICOM_LANGUAGE_INTELLIGENCE_WORKSPACE_ROUTING_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_ROUTING_API_VERSION 1U

typedef struct UmiLanguageIntelligenceWorkspaceRouting {
    uint32_t struct_size;
    uint32_t api_version;
    char source_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char target_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char scope_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    uint32_t priority;
    int enabled;
    uint64_t revision;
} UmiLanguageIntelligenceWorkspaceRouting;

void umi_language_intelligence_workspace_routing_init(UmiLanguageIntelligenceWorkspaceRouting *mapping);
UmiStatus umi_language_intelligence_workspace_routing_set(
    UmiLanguageIntelligenceWorkspaceRouting *mapping,
    const char *source_id,
    const char *target_id,
    const char *scope_id);
UmiStatus umi_language_intelligence_workspace_routing_validate(
    const UmiLanguageIntelligenceWorkspaceRouting *mapping);
int umi_language_intelligence_workspace_routing_matches(
    const UmiLanguageIntelligenceWorkspaceRouting *mapping,
    const char *source_id,
    const char *scope_id);
#ifdef __cplusplus
}
#endif
#endif
