/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_surface_registry.c
 *
 * PURPOSE:
 *   Focused regression coverage for IDE integration surface registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ide_integration/builtin_surfaces.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiIdeSurfaceRegistry *registry = NULL;
    assert(umi_ide_surface_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_ide_builtin_surfaces_install(registry) == UMI_STATUS_OK);
    assert(umi_ide_surface_registry_count(registry) == 16U);
    assert(umi_ide_surface_registry_find(
        registry, "umicom.ide.editor") != NULL);
    umi_ide_surface_registry_destroy(registry);
    return 0;
}

