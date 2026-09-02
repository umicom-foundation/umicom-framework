/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_declarative_catalogue.c
 *
 * PURPOSE:
 *   Verify the default catalogue exposes reusable semantic components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This test exercises the public contract rather than relying on private state,
 * making the behaviour useful as an executable example for new developers.
 */
#include "umicom/umicom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeclComponentRegistry *registry=NULL;
    UmiDeclComponentDescriptor descriptor;
    assert(umi_decl_component_registry_create(&registry)==UMI_STATUS_OK);
    assert(umi_decl_catalogue_register_defaults(registry)==UMI_STATUS_OK);
    assert(umi_decl_component_registry_count(registry)>=20U);
    assert(umi_decl_component_registry_find(registry,"editor",&descriptor)==UMI_STATUS_OK);
    assert(strcmp(descriptor.category,"Development")==0);
    umi_decl_component_registry_destroy(registry);
    return EXIT_SUCCESS;
}
