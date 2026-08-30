/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_catalogue_header_coexistence.c
 *
 * PURPOSE:
 *   Prove application definitions and experience profiles remain available
 *   when the profile catalogue is included before the definition catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stddef.h>

#include "umicom/application/experience/experience.h"
#include "umicom/application/experience_catalogue.h"

int main(void)
{
    assert(umi_application_experience_catalogue_find("org.umicom.studio") !=
           NULL);
    assert(umi_application_experience_profile_catalogue_for_recipe(
               "org.umicom.workspace.studio.standard") != NULL);
    return 0;
}
