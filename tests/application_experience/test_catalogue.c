/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_catalogue.c
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/application/experience_catalogue.h"

int test_experience_catalogue(void)
{
    const UmiApplicationExperienceDefinition *studio;
    const UmiApplicationExperienceDefinition *bank;
    assert(umi_application_experience_catalogue_validate() == UMI_STATUS_OK);
    assert(umi_application_experience_catalogue_count() >= 20U);
    studio = umi_application_experience_catalogue_find("org.umicom.studio");
    bank = umi_application_experience_catalogue_find("org.umicom.bank");
    assert(studio != NULL);
    assert(bank != NULL);
    assert(studio->layout_count >= 3U);
    assert(bank->panel_count >= 8U);
    return 0;
}
