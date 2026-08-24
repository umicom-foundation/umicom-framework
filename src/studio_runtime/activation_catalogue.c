/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_catalogue.c
 *
 * PURPOSE:
 *   Implement command-to-surface activation resolution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_catalogue.h"

typedef const UmiStudioRuntimeActivationProfile *(*ActivationFactory)(void);

static const ActivationFactory FACTORIES[] = {
    umi_studio_activation_profile_explorer,
    umi_studio_activation_profile_editor,
    umi_studio_activation_profile_outline,
    umi_studio_activation_profile_search,
    umi_studio_activation_profile_problems,
    umi_studio_activation_profile_output,
    umi_studio_activation_profile_terminal,
    umi_studio_activation_profile_source_control,
    umi_studio_activation_profile_test_explorer,
    umi_studio_activation_profile_debug,
    umi_studio_activation_profile_build,
    umi_studio_activation_profile_ai_chat,
    umi_studio_activation_profile_ai_review,
    umi_studio_activation_profile_ai_approvals,
    umi_studio_activation_profile_ai_tools,
    umi_studio_activation_profile_ai_validation
};

size_t umi_studio_activation_profile_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_at(size_t index)
{
    return index < umi_studio_activation_profile_count()
        ? FACTORIES[index]()
        : NULL;
}

UmiStatus umi_studio_activation_resolve(
    const char *command_id,
    UmiStudioRuntimeActivationRule *out_rule)
{
    size_t profile_index;
    int pass;

    if (command_id == NULL || out_rule == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Exact rules win over prefix rules. This prevents a broad "debug." or
     * "test." profile from shadowing a deliberately mapped command.
     */
    for (pass = 0; pass < 2; ++pass) {
        for (profile_index = 0U;
             profile_index < umi_studio_activation_profile_count();
             ++profile_index) {
            const UmiStudioRuntimeActivationProfile *profile =
                FACTORIES[profile_index]();
            size_t rule_index;

            if (profile == NULL) continue;

            for (rule_index = 0U;
                 rule_index < profile->rule_count;
                 ++rule_index) {
                const UmiStudioRuntimeActivationRule *rule =
                    &profile->rules[rule_index];

                if ((pass == 0 && rule->prefix_match) ||
                    (pass == 1 && !rule->prefix_match)) {
                    continue;
                }

                if (umi_studio_activation_rule_matches(rule, command_id)) {
                    *out_rule = *rule;
                    return UMI_STATUS_OK;
                }
            }
        }
    }

    return UMI_STATUS_NOT_FOUND;
}
