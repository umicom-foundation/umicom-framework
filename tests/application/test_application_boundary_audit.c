/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_application_boundary_audit.c
 *
 * PURPOSE:
 *   Prove the built-in portfolio conforms and direct application dependencies
 *   are rejected with explicit architecture evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/application.h"

int main(void)
{
    UmiApplicationAuditReport report;
    const UmiApplicationDefinition *applications[2];
    UmiApplicationDependency forbidden = {
        "org.umicom.studio",
        UMI_APPLICATION_DEPENDENCY_APPLICATION,
        "org.umicom.trader"
    };

    assert(umi_application_portfolio_audit(&report) == UMI_STATUS_OK);
    assert(report.passed);
    assert(report.error_count == 0U);
    assert(report.application_count == umi_application_portfolio_count());
    assert(report.capability_reference_count > report.application_count);

    applications[0] = umi_application_portfolio_find("org.umicom.studio");
    applications[1] = umi_application_portfolio_find("org.umicom.trader");
    assert(umi_application_boundary_audit(applications, 2U, &forbidden, 1U,
                                          &report) == UMI_STATUS_OK);
    assert(!report.passed);
    assert(report.error_count == 1U);
    assert(report.finding_count == 1U);
    return 0;
}
