/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_missing_application.c
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
#include "umicom/application/suite_layout/suite_layout.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiWorkspaceLayout l; UmiApplicationSuiteLayoutRuntime r;
    assert(umi_application_suite_layout_project_default("org.umicom.missing",&l)==UMI_STATUS_NOT_FOUND);
    umi_application_suite_layout_runtime_init(&r);
    assert(umi_application_suite_layout_runtime_load(&r,"org.umicom.missing")==UMI_STATUS_NOT_FOUND);
    return 0;
}
