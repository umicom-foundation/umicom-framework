/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_surface_manifest_closure.c
 *
 * PURPOSE:
 *   Integration regression coverage for surface manifest closure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ide_integration/builtin_surfaces.h"
#include "umicom/ide_integration/self_host_manifest.h"
int main(void)
{
    UmiIdeSurfaceRegistry *registry = NULL;
    const UmiIdeSelfHostManifest *manifest = umi_ide_self_host_manifest();
    size_t i;
    assert(umi_ide_surface_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_ide_builtin_surfaces_install(registry) == UMI_STATUS_OK);
    for (i=0U;i<manifest->required_surface_count;++i)
        assert(umi_ide_surface_registry_find(
            registry, manifest->required_surface_ids[i]) != NULL);
    umi_ide_surface_registry_destroy(registry);
    return 0;
}

