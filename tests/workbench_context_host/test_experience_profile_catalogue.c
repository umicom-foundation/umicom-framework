/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_experience_profile_catalogue.c
 *
 * PURPOSE:
 *   Verify every Framework-owned application experience receives a reusable
 *   workbench context profile, including applications with passive panels.
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
#include "umicom/workbench_context_host/experience_profile_catalogue.h"

int main(void)
{
    UmiWorkbenchContextHostProfileCatalogue catalogue;
    UmiWorkbenchContextHostExperienceProfileSummary summary;
    const UmiWorkbenchContextHostProfile *bank;
    const UmiWorkbenchContextHostProfile *os;

    umi_workbench_context_host_profile_catalogue_init(&catalogue);
    assert(umi_workbench_context_host_experience_profiles_register(
        &catalogue, NULL, &summary) == UMI_STATUS_OK);
    assert(summary.application_count ==
           umi_application_experience_catalogue_count());
    assert(catalogue.count == summary.application_count);
    assert(summary.endpoint_count ==
           summary.active_endpoint_count + summary.passive_endpoint_count);
    assert(summary.group_count > summary.application_count);

    bank = umi_workbench_context_host_profile_catalogue_find(
        &catalogue, "org.umicom.bank.experience-context");
    os = umi_workbench_context_host_profile_catalogue_find(
        &catalogue, "org.umicom.os.experience-context");
    assert(bank != NULL && bank->endpoint_count > 0U);
    assert(os != NULL && os->endpoint_count > 0U);

    umi_workbench_context_host_profile_catalogue_destroy(&catalogue);
    return 0;
}
