/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_model_registry.c
 *
 * PURPOSE:
 *   Verify multiple projects and active-project selection.
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

#include "umicom/developer_project/model_registry.h"

static void make_model(UmiDeveloperProjectModel *model,
                       const char *id,
                       const char *name)
{
    umi_developer_project_model_init(model, id, name);
    (void)strcpy(model->root, "C:/work");
    (void)strcpy(model->primary_language_id, "developer.language.c23");
    model->build_system = UMI_DEVELOPER_PROJECT_BUILD_CMAKE;
}

int main(void)
{
    UmiDeveloperProjectModelRegistry *registry = NULL;
    UmiDeveloperProjectModel alpha;
    UmiDeveloperProjectModel beta;
    UmiDeveloperProjectModel active;

    assert(umi_developer_project_model_registry_create(&registry) ==
           UMI_STATUS_OK);
    make_model(&alpha, "alpha", "Alpha");
    make_model(&beta, "beta", "Beta");

    assert(umi_developer_project_model_registry_upsert(
        registry, &alpha) == UMI_STATUS_OK);
    assert(umi_developer_project_model_registry_upsert(
        registry, &beta) == UMI_STATUS_OK);
    assert(umi_developer_project_model_registry_activate(
        registry, "beta") == UMI_STATUS_OK);
    assert(umi_developer_project_model_registry_active(
        registry, &active) == UMI_STATUS_OK);
    assert(strcmp(active.project_id, "beta") == 0);

    umi_developer_project_model_registry_destroy(registry);
    return 0;
}
