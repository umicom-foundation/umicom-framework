/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/helix/proposal_validator.c
 *
 * PURPOSE:
 *   Enforce role/action/configuration limits before a model proposal reaches Helix.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/helix/proposal_validator.h"
#include "umicom/ai/helix/role_profile.h"
#include <string.h>

UmiStatus umi_ai_helix_proposal_validate(
    const UmiAiHelixBridgeConfig *config,
    UmiHelixAgentRole role,
    const UmiAiHelixParsedResponse *parsed)
{
    uint32_t action_mask;
    UmiStatus status;
    if (config == NULL || parsed == NULL ||
        umi_ai_helix_role_profile_find(role) == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ai_helix_bridge_config_validate(config);
    if (status != UMI_STATUS_OK) return status;
    if (parsed->confidence < 0.0 || parsed->confidence > 1.0 ||
        parsed->summary[0] == '\0' || parsed->rationale[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (parsed->disposition != UMI_AI_HELIX_DISPOSITION_PROPOSE_ACTION) {
        return (uint32_t)parsed->action_kind == 0U ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
    }
    action_mask = umi_ai_helix_action_mask(parsed->action_kind);
    if (action_mask == 0U || parsed->target[0] == '\0' ||
        strcmp(parsed->target, "-") == 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((config->allowed_actions & action_mask) == 0U ||
        !umi_ai_helix_role_allows_action(role, parsed->action_kind)) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (parsed->action_kind == UMI_HELIX_ACTION_SOURCE_CONTROL &&
        !config->allow_source_control) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return UMI_STATUS_OK;
}
