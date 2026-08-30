/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_studio.c
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
#include <string.h>
#include "umicom/application/suite_layout/suite_layout.h"
int main(void) {
    UmiApplicationSuiteLayoutRuntime r; UmiApplicationSuiteLayoutSnapshot s;
    umi_application_suite_layout_runtime_init(&r);
    assert(umi_application_suite_layout_runtime_load(&r,"org.umicom.studio")==UMI_STATUS_OK);
    s=umi_application_suite_layout_runtime_snapshot(&r);
    assert(s.loaded); assert(s.window_count==8U); assert(strstr(s.active_layout_id,"development")!=NULL);
    assert(umi_application_suite_layout_runtime_select(&r,"review")==UMI_STATUS_OK);
    assert(umi_application_suite_layout_runtime_active(&r)->window_count==6U);
    return 0;
}
