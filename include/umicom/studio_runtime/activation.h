/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/activation.h
 *
 * PURPOSE:
 *   Map successful runtime commands to the Studio surface that should become
 *   visible/active/focused after the command executes.
 *
 * DESIGN:
 *   Command execution remains authoritative in UmiCommandRegistry. Activation
 *   rules only affect shell presentation after successful execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ACTIVATION_H
#define UMICOM_STUDIO_RUNTIME_ACTIVATION_H

#include "umicom/studio_runtime/surface_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the studio runtime activation rule data shared with callers of this public
 * contract.
 */
typedef struct UmiStudioRuntimeActivationRule {
    const char *command_pattern;
    UmiStudioRuntimeSurfaceKind surface;
    int prefix_match;
    int make_visible;
    int focus;
} UmiStudioRuntimeActivationRule;

/**
 * Represent the studio runtime activation profile data shared with callers of this public
 * contract.
 */
typedef struct UmiStudioRuntimeActivationProfile {
    const char *profile_id;
    const UmiStudioRuntimeActivationRule *rules;
    size_t rule_count;
} UmiStudioRuntimeActivationProfile;

/**
 * Check that studio activation rule satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_studio_activation_rule_validate(
    const UmiStudioRuntimeActivationRule *rule);

/**
 * Provide the studio activation rule matches operation used by this module and its client
 * applications.
 */
int umi_studio_activation_rule_matches(
    const UmiStudioRuntimeActivationRule *rule,
    const char *command_id);

#ifdef __cplusplus
}
#endif
#endif
