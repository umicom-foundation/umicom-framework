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

#include <stdlib.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/suite_layout.h"
#include "umicom/test_runtime/check.h"

/* Build the suite-wide index and prove every application layout is covered. */
int main(void)
{
    UmiApplicationSuiteLayoutCatalogue *catalogue =
        (UmiApplicationSuiteLayoutCatalogue *)calloc(1U, sizeof(*catalogue));
    size_t expected_layout_count = 0U;
    size_t index;

    UMI_TEST_REQUIRE(catalogue != NULL);
    UMI_TEST_REQUIRE(umi_application_suite_layout_catalogue_build(catalogue) ==
                     UMI_STATUS_OK);
    UMI_TEST_REQUIRE(catalogue->count ==
                     umi_application_experience_catalogue_count());
    for (index = 0U;
         index < umi_application_experience_catalogue_count();
         ++index) {
        const UmiApplicationExperienceDefinition *experience =
            umi_application_experience_catalogue_at(index);
        UMI_TEST_REQUIRE(experience != NULL);
        expected_layout_count += experience->layout_count;
    }
    UMI_TEST_REQUIRE(catalogue->layout_count == expected_layout_count);
    UMI_TEST_REQUIRE(catalogue->lockable_layout_count > 0U);
    UMI_TEST_REQUIRE(catalogue->multi_monitor_layout_count > 0U);
    UMI_TEST_REQUIRE(catalogue->context_linked_layout_count > 0U);
    UMI_TEST_REQUIRE(catalogue->complete_count == catalogue->count);
    UMI_TEST_REQUIRE(catalogue->incomplete_count == 0U);
    free(catalogue);
    return 0;
}
