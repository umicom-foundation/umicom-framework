/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/application_scaffold.c
 * PURPOSE: Keep application-module adoption independent of Studio and GTK.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/developer_project/application_scaffold.h"
#include "umicom/base/text.h"
#include <stdlib.h>

/* Build the two large records on the heap. This keeps worker stacks bounded and
 * publishes a complete pair only after all capacity and model checks succeed. */
UmiStatus UmiDeveloperApplicationScaffoldProject(const UmiApplicationScaffoldPlan *plan,
    UmiDeveloperProjectModel *outModel, UmiBuildProfile *outProfile)
{
    if (plan == NULL || outModel == NULL || outProfile == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiApplicationScaffoldSummary *summary = calloc(1U, sizeof *summary);
    UmiDeveloperProjectModel *model = calloc(1U, sizeof *model);
    UmiBuildProfile *profile = calloc(1U, sizeof *profile);
    if (summary == NULL || model == NULL || profile == NULL) {
        free(summary); free(model); free(profile);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus status = umi_application_scaffold_summary(plan, summary);
    umi_developer_project_model_init(model, NULL, NULL);
    umi_build_profile_init(profile);
#define COPY(destination, source) \
    if (status == UMI_STATUS_OK) \
        status = umi_text_copy(destination, sizeof(destination), source)
    COPY(model->project_id, summary->module.id);
    COPY(model->display_name, summary->module.name);
    COPY(model->root, summary->root);
    COPY(model->template_id, "umicom.application.module");
    COPY(model->primary_language_id, "c");
    COPY(model->entry_point, summary->module.native_executable[0] != '\0'
        ? "src/gtk.c" : "src/console.c");
    COPY(model->build_directory, "build/umicom-development");
    COPY(model->install_prefix, "install/umicom-development");
    COPY(profile->profile_id, "project.development");
    COPY(profile->source_directory, model->root);
    COPY(profile->build_directory, model->build_directory);
    COPY(profile->install_directory, model->install_prefix);
#undef COPY
    /* A file preview is not execution consent or proof that a compiler exists.
     * The host applies its normal workspace trust and build-settings workflow. */
    profile->parallel_jobs = 2U;
    profile->run_program[0] = '\0';
    if (status == UMI_STATUS_OK)
        status = umi_developer_project_model_validate(model, NULL, 0U);
    if (status == UMI_STATUS_OK) status = umi_build_profile_validate(profile, NULL, 0U);
    if (status == UMI_STATUS_OK) {
        *outModel = *model;
        *outProfile = *profile;
    }
    free(summary); free(model); free(profile);
    return status;
}
