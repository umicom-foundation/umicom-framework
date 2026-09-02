/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_activation_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime activation catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/studio_runtime/activation_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiStudioRuntimeActivationRule rule;
    assert(umi_studio_activation_profile_count() == 19U);
    assert(umi_studio_activation_resolve(
        "ide.open.problem", &rule) == UMI_STATUS_OK);
    assert(rule.surface == UMI_STUDIO_SURFACE_EDITOR);
    assert(umi_studio_activation_resolve(
        "ai.developer.open-approvals", &rule) == UMI_STATUS_OK);
    assert(rule.surface == UMI_STUDIO_SURFACE_AI_APPROVALS);
    assert(umi_studio_activation_resolve(
        "ai.developer.open-model-comparison", &rule) == UMI_STATUS_OK);
    assert(rule.surface == UMI_STUDIO_SURFACE_AI_MODEL_COMPARISON);
    assert(umi_studio_activation_resolve(
        "help.ai-teacher", &rule) == UMI_STATUS_OK);
    assert(rule.surface == UMI_STUDIO_SURFACE_LEARNING);
    assert(umi_studio_activation_resolve(
        "help.documentation", &rule) == UMI_STATUS_OK);
    assert(rule.surface == UMI_STUDIO_SURFACE_DOCUMENTATION);
    return 0;
}
