/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_surface_install.c
 *
 * PURPOSE:
 *   Behavioral regression coverage for Studio runtime surface install.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdlib.h>

#include "umicom/test_runtime/check.h"
#include "umicom/studio_runtime/surface_catalogue.h"
int main(void)
{
    UmiApplicationShellRegistry *registry=NULL;
    UmiApplicationShellLayout *layout =
        (UmiApplicationShellLayout *)calloc(1U, sizeof(*layout));
    UmiApplicationShellContribution contribution;
    UMI_TEST_REQUIRE(layout != NULL);
    UMI_TEST_REQUIRE(umi_application_shell_registry_create(&registry)==UMI_STATUS_OK);
    umi_application_shell_layout_init(layout,"layout","Layout");
    UMI_TEST_REQUIRE(umi_studio_surface_catalogue_install(registry,layout)==UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_studio_surface_catalogue_count() > 0U);
    /* Several semantic Studio surfaces may intentionally resolve to one shared
     * physical contribution. Every installed contribution still needs exactly
     * one layout placement, regardless of catalogue growth. */
    UMI_TEST_REQUIRE(layout->placement_count ==
                     umi_application_shell_registry_count(registry));
    UMI_TEST_REQUIRE(umi_studio_surface_catalogue_count() >=
                     layout->placement_count);
    UMI_TEST_REQUIRE(umi_studio_runtime_surface_resolve(
        registry,umi_studio_surface_profile_editor(),&contribution)==UMI_STATUS_OK);
    UMI_TEST_REQUIRE(contribution.role==UMI_APPLICATION_SHELL_ROLE_EDITOR);
    umi_application_shell_registry_destroy(registry);
    free(layout);
    return 0;
}
