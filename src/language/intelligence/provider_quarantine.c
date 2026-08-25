/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/provider_quarantine.c
 *
 * PURPOSE:
 *   Implement quarantine unstable providers and allow explicit controlled release.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/provider_quarantine.h"
#include <string.h>

void umi_language_intelligence_provider_quarantine_init(UmiLanguageIntelligenceProviderQuarantine *value, const char *id)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_LANGUAGE_INTELLIGENCE_PROVIDER_QUARANTINE_API_VERSION;
    value->state = UMI_LANGUAGE_INTELLIGENCE_STOPPED;
    value->generation = 1U;
    value->revision = 1U;
    if (id != NULL) {
        (void)umi_language_intelligence_copy_text(value->id, sizeof(value->id), id);
    }
}

UmiStatus umi_language_intelligence_provider_quarantine_transition(
    UmiLanguageIntelligenceProviderQuarantine *value,
    UmiLanguageIntelligenceState next_state,
    int explicit_release)
{
    if (value == NULL || value->id[0] == '\0' ||
        next_state < UMI_LANGUAGE_INTELLIGENCE_UNKNOWN ||
        next_state > UMI_LANGUAGE_INTELLIGENCE_FAILED)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (value->state == UMI_LANGUAGE_INTELLIGENCE_QUARANTINED &&
        next_state != UMI_LANGUAGE_INTELLIGENCE_STOPPED &&
        explicit_release == 0) return UMI_STATUS_PERMISSION_DENIED;
    if (value->state == UMI_LANGUAGE_INTELLIGENCE_FAILED &&
        next_state == UMI_LANGUAGE_INTELLIGENCE_READY)
        return UMI_STATUS_INVALID_STATE;
    if (value->state == UMI_LANGUAGE_INTELLIGENCE_STOPPED &&
        next_state == UMI_LANGUAGE_INTELLIGENCE_READY)
        return UMI_STATUS_INVALID_STATE;
    if (next_state == UMI_LANGUAGE_INTELLIGENCE_STARTING) {
        if (value->generation != UINT64_MAX) value->generation += 1U;
        if (value->restart_count != UINT32_MAX) value->restart_count += 1U;
    }
    value->state = next_state;
    if (value->revision != UINT64_MAX) value->revision += 1U;
    return UMI_STATUS_OK;
}

void umi_language_intelligence_provider_quarantine_record_failure(UmiLanguageIntelligenceProviderQuarantine *value)
{
    if (value == NULL) return;
    if (value->failure_count != UINT32_MAX) value->failure_count += 1U;
    value->state = value->failure_count >= 3U
        ? UMI_LANGUAGE_INTELLIGENCE_QUARANTINED
        : UMI_LANGUAGE_INTELLIGENCE_DEGRADED;
    if (value->revision != UINT64_MAX) value->revision += 1U;
}

int umi_language_intelligence_provider_quarantine_is_usable(const UmiLanguageIntelligenceProviderQuarantine *value)
{
    return value != NULL &&
        (value->state == UMI_LANGUAGE_INTELLIGENCE_READY ||
         value->state == UMI_LANGUAGE_INTELLIGENCE_DEGRADED);
}
