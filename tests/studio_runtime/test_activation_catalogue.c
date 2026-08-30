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
int main(void)
{
    UmiStudioRuntimeActivationRule rule;
    assert(umi_studio_activation_profile_count() == 16U);
    assert(umi_studio_activation_resolve(
        "ide.open.problem", &rule) == UMI_STATUS_OK);
    assert(rule.surface == UMI_STUDIO_SURFACE_EDITOR);
    assert(umi_studio_activation_resolve(
        "ai.developer.open-approvals", &rule) == UMI_STATUS_OK);
    assert(rule.surface == UMI_STUDIO_SURFACE_AI_APPROVALS);
    return 0;
}

