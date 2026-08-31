/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation.c
 *
 * PURPOSE:
 *   Implement deterministic exact/prefix command activation matching.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation.h"

#include <string.h>

UmiStatus umi_studio_activation_rule_validate(
    const UmiStudioRuntimeActivationRule *rule)
{
    if (rule == NULL ||
        rule->command_pattern == NULL ||
        rule->command_pattern[0] == '\0' ||
        rule->surface < UMI_STUDIO_SURFACE_EXPLORER ||
        rule->surface > UMI_STUDIO_SURFACE_LAST) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}

int umi_studio_activation_rule_matches(
    const UmiStudioRuntimeActivationRule *rule,
    const char *command_id)
{
    size_t length;

    if (umi_studio_activation_rule_validate(rule) != UMI_STATUS_OK ||
        command_id == NULL) {
        return 0;
    }

    if (!rule->prefix_match) {
        return strcmp(rule->command_pattern, command_id) == 0;
    }

    length = strlen(rule->command_pattern);
    return strncmp(rule->command_pattern, command_id, length) == 0;
}
