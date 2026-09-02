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

/*
 * Check that studio activation rule satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_studio_activation_rule_validate(
    const UmiStudioRuntimeActivationRule *rule)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (rule == NULL ||
        rule->command_pattern == NULL ||
        rule->command_pattern[0] == '\0' ||
        rule->surface < UMI_STUDIO_SURFACE_EXPLORER ||
        rule->surface > UMI_STUDIO_SURFACE_LAST) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the studio activation rule matches operation used by this module and its client
 * applications.
 */
int umi_studio_activation_rule_matches(
    const UmiStudioRuntimeActivationRule *rule,
    const char *command_id)
{
    size_t length;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_studio_activation_rule_validate(rule) != UMI_STATUS_OK ||
        command_id == NULL) {
        return 0;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!rule->prefix_match) {
        return strcmp(rule->command_pattern, command_id) == 0;
    }

    length = strlen(rule->command_pattern);
    return strncmp(rule->command_pattern, command_id, length) == 0;
}
