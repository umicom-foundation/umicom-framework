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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ACTIVATION_H
#define UMICOM_STUDIO_RUNTIME_ACTIVATION_H

#include "umicom/studio_runtime/surface_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioRuntimeActivationRule {
    const char *command_pattern;
    UmiStudioRuntimeSurfaceKind surface;
    int prefix_match;
    int make_visible;
    int focus;
} UmiStudioRuntimeActivationRule;

typedef struct UmiStudioRuntimeActivationProfile {
    const char *profile_id;
    const UmiStudioRuntimeActivationRule *rules;
    size_t rule_count;
} UmiStudioRuntimeActivationProfile;

UmiStatus umi_studio_activation_rule_validate(
    const UmiStudioRuntimeActivationRule *rule);

int umi_studio_activation_rule_matches(
    const UmiStudioRuntimeActivationRule *rule,
    const char *command_id);

#ifdef __cplusplus
}
#endif
#endif
