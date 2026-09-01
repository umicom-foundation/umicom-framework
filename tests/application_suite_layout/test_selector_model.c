/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_selector_model.c
 * PURPOSE: Verify canonical Studio and Trader layouts share one selector model.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_runtime/check.h"
#include <string.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/selector_model.h"
int main(void)
{
    const UmiApplicationExperienceDefinition *experience =
        umi_application_experience_catalogue_find("org.umicom.trader");
    const UmiExperienceLayoutDefinition *current;
    UmiApplicationSuiteLayoutSelectorModel model;
    UMI_TEST_REQUIRE(experience != NULL);
    UMI_TEST_REQUIRE(umi_application_suite_layout_selector_build(
        experience, NULL, &model) == UMI_STATUS_OK);
    /* The selector must track catalogue growth without a stale literal count. */
    UMI_TEST_REQUIRE(model.count == experience->layout_count);
    /* Resolve and check the pointer separately so a malformed selector emits a
     * readable failed condition instead of dereferencing a null result. */
    current = umi_application_suite_layout_selector_current(&model);
    UMI_TEST_REQUIRE(current != NULL);
    UMI_TEST_REQUIRE(strcmp(current->layout_id, "trading") == 0);
    UMI_TEST_REQUIRE(umi_application_suite_layout_selector_select(
        &model, "research") == UMI_STATUS_OK);
    current = umi_application_suite_layout_selector_current(&model);
    UMI_TEST_REQUIRE(current != NULL);
    UMI_TEST_REQUIRE(strcmp(current->layout_id, "research") == 0);
    return 0;
}
