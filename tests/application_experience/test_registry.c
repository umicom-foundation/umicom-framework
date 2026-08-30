/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_registry.c
 *
 * PURPOSE:
 *   Implement the test registry behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_registry.c
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/experience_registry.h"

int test_experience_registry(void)
{
    UmiApplicationExperienceRegistry registry;
    const UmiApplicationExperienceDefinition *trader =
        umi_application_experience_catalogue_find("org.umicom.trader");
    umi_application_experience_registry_init(&registry);
    assert(umi_application_experience_registry_register(
        &registry, trader) == UMI_STATUS_OK);
    assert(umi_application_experience_registry_register(
        &registry, trader) == UMI_STATUS_ALREADY_EXISTS);
    assert(umi_application_experience_registry_count(&registry) == 1U);
    assert(umi_application_experience_registry_find(
        &registry, "org.umicom.trader") == trader);
    return 0;
}
