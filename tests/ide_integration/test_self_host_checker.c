/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_self_host_checker.c
 *
 * PURPOSE:
 *   Integration regression coverage for self host checker.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ide_integration/self_host_checker.h"
#include "umicom/ide_integration/builtin_surfaces.h"
static int all_capabilities(void *u,const char *id)
{(void)u;(void)id;return 1;}
int main(void)
{
    UmiIdeSurfaceRegistry *surfaces = NULL;
    UmiCommandRegistry *commands = NULL;
    UmiIdeSelfHostCheckReport report;
    assert(umi_ide_surface_registry_create(&surfaces) == UMI_STATUS_OK);
    assert(umi_ide_builtin_surfaces_install(surfaces) == UMI_STATUS_OK);
    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_ide_self_host_check(
        umi_ide_self_host_manifest(), surfaces, commands,
        all_capabilities, NULL, &report) == UMI_STATUS_OK);
    assert(!report.ready);
    assert(report.missing_command_count > 0U);
    umi_command_registry_destroy(commands);
    umi_ide_surface_registry_destroy(surfaces);
    return 0;
}

