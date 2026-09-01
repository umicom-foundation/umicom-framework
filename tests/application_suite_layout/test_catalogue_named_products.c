/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_catalogue_named_products.c
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

#include <stdlib.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/suite_layout.h"
#include "umicom/test_runtime/check.h"

/* Confirm named applications remain discoverable without placing the expanded
 * suite catalogue on a small native test stack. */
int main(void)
{
    UmiApplicationSuiteLayoutCatalogue *catalogue =
        (UmiApplicationSuiteLayoutCatalogue *)calloc(1U, sizeof(*catalogue));

    UMI_TEST_REQUIRE(catalogue != NULL);
    UMI_TEST_REQUIRE(umi_application_suite_layout_catalogue_build(catalogue) ==
                     UMI_STATUS_OK);
    UMI_TEST_REQUIRE(catalogue->count ==
                     umi_application_experience_catalogue_count());
    UMI_TEST_REQUIRE(umi_application_suite_layout_catalogue_find(
        catalogue, "org.umicom.studio") != NULL);
    UMI_TEST_REQUIRE(umi_application_suite_layout_catalogue_find(
        catalogue, "org.umicom.trader") != NULL);
    free(catalogue);
    return 0;
}
