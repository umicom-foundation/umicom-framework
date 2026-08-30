/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_model.c
 *
 * PURPOSE:
 *   Verify canonical project-model validation and multi-language state.
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

#include "umicom/developer_project/model.h"

int main(void)
{
    UmiDeveloperProjectModel model;

    umi_developer_project_model_init(&model, "alpha", "Alpha");
    (void)strcpy(model.root, "C:/work/alpha");
    (void)strcpy(model.primary_language_id, "developer.language.c23");
    model.build_system = UMI_DEVELOPER_PROJECT_BUILD_CMAKE;

    assert(umi_developer_project_model_add_language(
        &model, "developer.language.assembly") == UMI_STATUS_OK);
    assert(model.secondary_language_count == 1U);
    assert(umi_developer_project_model_validate(
        &model, NULL, 0U) == UMI_STATUS_OK);
    return 0;
}
