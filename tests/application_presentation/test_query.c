/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_query.c
 *
 * PURPOSE:
 *   Verify applications and frontend adapters can query projected panels by
 *   layout region without reimplementing traversal logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/presentation.h"

#include <assert.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationPresentationPlan plan;
    const UmiApplicationPresentationPanelPlacement *primary;
    size_t primary_count;

    assert(umi_application_presentation_project(
               "org.umicom.workspace.trader.standard", &plan) == UMI_STATUS_OK);
    primary_count = umi_application_presentation_plan_region_count(
        &plan, UMI_APPLICATION_COMPONENT_REGION_PRIMARY);
    assert(primary_count > 0U);
    primary = umi_application_presentation_plan_region_at(
        &plan, UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 0U);
    assert(primary != NULL);
    assert(primary->slot->region == UMI_APPLICATION_COMPONENT_REGION_PRIMARY);
    assert(umi_application_presentation_plan_region_at(
               &plan, UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
               primary_count) == NULL);
    return 0;
}
