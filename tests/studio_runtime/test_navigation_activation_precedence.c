/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_navigation_activation_precedence.c
 *
 * PURPOSE:
 *   Ensure commands that navigate from Tests, Source Control and Debug into
 *   source code activate the editor rather than refocusing the originating tool.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio_runtime/activation_catalogue.h"

int main(void)
{
    UmiStudioRuntimeActivationRule rule;

    assert(umi_studio_activation_resolve(
        "ide.open.test", &rule) == UMI_STATUS_OK);
    assert(rule.surface == UMI_STUDIO_SURFACE_EDITOR);

    assert(umi_studio_activation_resolve(
        "ide.open.source-control", &rule) == UMI_STATUS_OK);
    assert(rule.surface == UMI_STUDIO_SURFACE_EDITOR);

    assert(umi_studio_activation_resolve(
        "ide.open.debug-frame", &rule) == UMI_STATUS_OK);
    assert(rule.surface == UMI_STUDIO_SURFACE_EDITOR);

    return 0;
}
