/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_invalid_layout.c
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
    UmiApplicationSuiteLayoutRuntime r; umi_application_suite_layout_runtime_init(&r);
    assert(umi_application_suite_layout_runtime_load(&r,"org.umicom.studio")==UMI_STATUS_OK);
    assert(umi_application_suite_layout_runtime_select(&r,"missing-layout")==UMI_STATUS_NOT_FOUND);
    return 0;
}
