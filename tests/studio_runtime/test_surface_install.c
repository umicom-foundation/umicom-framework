/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_surface_install.c
 *
 * PURPOSE:
 *   Behavioral regression coverage for Studio runtime surface install.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/studio_runtime/surface_catalogue.h"
int main(void)
{
    UmiApplicationShellRegistry *registry=NULL;
    UmiApplicationShellLayout layout;
    UmiApplicationShellContribution contribution;
    assert(umi_application_shell_registry_create(&registry)==UMI_STATUS_OK);
    umi_application_shell_layout_init(&layout,"layout","Layout");
    assert(umi_studio_surface_catalogue_install(registry,&layout)==UMI_STATUS_OK);
    assert(umi_studio_surface_catalogue_count()==16U);
    assert(layout.placement_count==16U);
    assert(umi_studio_runtime_surface_resolve(
        registry,umi_studio_surface_profile_editor(),&contribution)==UMI_STATUS_OK);
    assert(contribution.role==UMI_APPLICATION_SHELL_ROLE_EDITOR);
    umi_application_shell_registry_destroy(registry);
    return 0;
}

