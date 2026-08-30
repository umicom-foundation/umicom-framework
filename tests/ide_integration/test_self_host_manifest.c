/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_self_host_manifest.c
 *
 * PURPOSE:
 *   Focused regression coverage for IDE integration self host manifest.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ide_integration/self_host_manifest.h"
int main(void)
{
    const UmiIdeSelfHostManifest *manifest = umi_ide_self_host_manifest();
    assert(manifest != NULL);
    assert(manifest->required_surface_count >= 10U);
    assert(manifest->required_command_count >= 10U);
    assert(umi_ide_self_host_manifest_validate(manifest) == UMI_STATUS_OK);
    return 0;
}

