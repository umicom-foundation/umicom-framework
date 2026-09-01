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
#include <assert.h>
#include <string.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/selector_model.h"
int main(void)
{
    const UmiApplicationExperienceDefinition *experience =
        umi_application_experience_catalogue_find("org.umicom.trader");
    UmiApplicationSuiteLayoutSelectorModel model;
    assert(experience != NULL);
    assert(umi_application_suite_layout_selector_build(
        experience, NULL, &model) == UMI_STATUS_OK);
    /* The selector must track catalogue growth without a stale literal count. */
    assert(model.count == experience->layout_count);
    assert(strcmp(umi_application_suite_layout_selector_current(&model)->layout_id,
                  "trading") == 0);
    assert(umi_application_suite_layout_selector_select(
        &model, "research") == UMI_STATUS_OK);
    assert(strcmp(umi_application_suite_layout_selector_current(&model)->layout_id,
                  "research") == 0);
    return 0;
}
