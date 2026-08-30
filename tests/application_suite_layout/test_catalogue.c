/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_catalogue.c
 *
 * PURPOSE:
 *   Exercise Framework-owned Application Suite layout materialisation and coverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/suite_layout.h"
int main(void) {
    UmiApplicationSuiteLayoutCatalogue catalogue;
    assert(umi_application_suite_layout_catalogue_build(&catalogue) == UMI_STATUS_OK);
    assert(catalogue.count == umi_application_experience_catalogue_count());
    assert(catalogue.complete_count == catalogue.count);
    assert(catalogue.incomplete_count == 0U);
    return 0;
}
