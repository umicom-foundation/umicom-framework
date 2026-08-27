/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_regions.c
 *
 * PURPOSE:
 *   Exercise Framework-owned Application Suite layout materialisation and coverage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/application/suite_layout/suite_layout.h"
int main(void) {
    UmiApplicationSuiteLayoutRect l=umi_application_suite_layout_region_rect(UMI_UI_PLACEMENT_LEFT);
    UmiApplicationSuiteLayoutRect c=umi_application_suite_layout_region_rect(UMI_UI_PLACEMENT_CENTRE);
    UmiApplicationSuiteLayoutRect r=umi_application_suite_layout_region_rect(UMI_UI_PLACEMENT_RIGHT);
    UmiApplicationSuiteLayoutRect b=umi_application_suite_layout_region_rect(UMI_UI_PLACEMENT_BOTTOM);
    assert(l.x==0.0 && l.width>0.0); assert(c.x>l.x); assert(r.x>c.x); assert(b.y>0.0);
    return 0;
}
