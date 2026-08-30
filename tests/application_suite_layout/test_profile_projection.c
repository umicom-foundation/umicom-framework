/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_profile_projection.c
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
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/suite_layout.h"
int main(void) {
    const UmiApplicationExperienceDefinition *e=umi_application_experience_catalogue_find("org.umicom.studio");
    const UmiExperienceLayoutDefinition *l;
    UmiUiWorkspaceProfileSnapshot p;
    assert(e!=NULL); l=umi_application_experience_layout_find(e,e->default_layout_id); assert(l!=NULL);
    assert(umi_application_suite_layout_profile_project(e,l,100,&p)==UMI_STATUS_OK);
    assert(p.sidebar_visible); assert(p.auxiliary_sidebar_visible); assert(p.bottom_panel_visible);
    assert(p.pane_count>0U);
    return 0;
}
