/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_all_defaults.c
 *
 * PURPOSE:
 *   Exercise Framework-owned Application Suite layout materialisation and coverage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/suite_layout.h"
int main(void) {
    size_t i;
    for (i=0U;i<umi_application_experience_catalogue_count();++i) {
        const UmiApplicationExperienceDefinition *e=umi_application_experience_catalogue_at(i);
        UmiUiWorkspaceLayout l;
        assert(e != NULL);
        assert(umi_application_suite_layout_project_default(e->application_id,&l)==UMI_STATUS_OK);
        assert(l.window_count > 0U);
    }
    return 0;
}
