/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_selector_invalid.c
 * PURPOSE: Verify layout selector rejects unknown active and requested layouts.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/selector_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiApplicationExperienceDefinition *experience =
        umi_application_experience_catalogue_find("org.umicom.studio");
    UmiApplicationSuiteLayoutSelectorModel model;
    assert(experience != NULL);
    assert(umi_application_suite_layout_selector_build(
        experience, "missing", &model) == UMI_STATUS_NOT_FOUND);
    assert(umi_application_suite_layout_selector_build(
        experience, NULL, &model) == UMI_STATUS_OK);
    assert(umi_application_suite_layout_selector_select(
        &model, "missing") == UMI_STATUS_NOT_FOUND);
    return 0;
}
