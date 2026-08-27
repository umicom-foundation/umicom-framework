/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_catalogue_named_products.c
 *
 * PURPOSE:
 *   Exercise Framework-owned Application Suite layout materialisation and coverage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/suite_layout.h"
int main(void) {
    UmiApplicationSuiteLayoutCatalogue c;
    assert(umi_application_suite_layout_catalogue_build(&c)==UMI_STATUS_OK);
    assert(c.count==umi_application_experience_catalogue_count());
    assert(c.count==25U);
    assert(umi_application_suite_layout_catalogue_find(&c,"org.umicom.studio")!=NULL);
    assert(umi_application_suite_layout_catalogue_find(&c,"org.umicom.trader")!=NULL);
    return 0;
}
