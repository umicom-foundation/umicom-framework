/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/workspace_routing.c
 *
 * PURPOSE:
 *   Implement route workspace requests to the appropriate language-service provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/workspace_routing.h"
#include <string.h>

void umi_language_intelligence_workspace_routing_init(UmiLanguageIntelligenceWorkspaceRouting *mapping)
{
    if (mapping == NULL) return;
    (void)memset(mapping, 0, sizeof(*mapping));
    mapping->struct_size = (uint32_t)sizeof(*mapping);
    mapping->api_version = UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_ROUTING_API_VERSION;
    mapping->enabled = 1;
    mapping->revision = 1U;
}

UmiStatus umi_language_intelligence_workspace_routing_set(
    UmiLanguageIntelligenceWorkspaceRouting *mapping,
    const char *source_id,
    const char *target_id,
    const char *scope_id)
{
    UmiStatus status;
    if (mapping == NULL || source_id == NULL || target_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_language_intelligence_copy_text(
        mapping->source_id, sizeof(mapping->source_id), source_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_language_intelligence_copy_text(
        mapping->target_id, sizeof(mapping->target_id), target_id);
    if (status != UMI_STATUS_OK) return status;
    mapping->scope_id[0] = '\0';
    if (scope_id != NULL && scope_id[0] != '\0') {
        status = umi_language_intelligence_copy_text(
            mapping->scope_id, sizeof(mapping->scope_id), scope_id);
        if (status != UMI_STATUS_OK) return status;
    }
    if (mapping->revision != UINT64_MAX) mapping->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_language_intelligence_workspace_routing_validate(
    const UmiLanguageIntelligenceWorkspaceRouting *mapping)
{
    if (mapping == NULL ||
        mapping->struct_size < sizeof(*mapping) ||
        mapping->api_version != UMI_LANGUAGE_INTELLIGENCE_WORKSPACE_ROUTING_API_VERSION ||
        mapping->source_id[0] == '\0' || mapping->target_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

int umi_language_intelligence_workspace_routing_matches(
    const UmiLanguageIntelligenceWorkspaceRouting *mapping,
    const char *source_id,
    const char *scope_id)
{
    if (umi_language_intelligence_workspace_routing_validate(mapping) != UMI_STATUS_OK ||
        source_id == NULL || strcmp(mapping->source_id, source_id) != 0 ||
        mapping->enabled == 0) return 0;
    if (mapping->scope_id[0] == '\0') return 1;
    return scope_id != NULL && strcmp(mapping->scope_id, scope_id) == 0;
}
