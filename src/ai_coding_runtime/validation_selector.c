/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/validation_selector.c
 *
 * PURPOSE:
 *   Implement marker-based validation profile detection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/validation_selector.h"

#include <stdio.h>
#include <string.h>

typedef UmiStatus (*ProfileFactory)(
    const char *workspace_root,
    UmiAiCodingValidationPlan *out_plan);

typedef struct Candidate {
    const char *profile_id;
    const char *marker;
    ProfileFactory factory;
} Candidate;

static const Candidate CANDIDATES[] = {
    {"cmake", "CMakeLists.txt", umi_ai_coding_validation_profile_cmake},
    {"meson", "meson.build", umi_ai_coding_validation_profile_meson},
    {"cargo", "Cargo.toml", umi_ai_coding_validation_profile_cargo},
    {"zig", "build.zig", umi_ai_coding_validation_profile_zig},
    {"go", "go.mod", umi_ai_coding_validation_profile_go},
    {"gradle", "gradlew", umi_ai_coding_validation_profile_gradle},
    {"gradle", "build.gradle", umi_ai_coding_validation_profile_gradle},
    {"maven", "pom.xml", umi_ai_coding_validation_profile_maven},
    {"npm", "package.json", umi_ai_coding_validation_profile_npm},
    {"python", "pyproject.toml", umi_ai_coding_validation_profile_python},
    {"python", "setup.py", umi_ai_coding_validation_profile_python},
    {"make", "Makefile", umi_ai_coding_validation_profile_make}
};

UmiStatus umi_ai_coding_validation_select(
    const UmiAiCodingWorkspaceAdapter *workspace,
    const char *workspace_root,
    UmiAiCodingValidationSelection *out_selection,
    UmiAiCodingValidationPlan *out_plan)
{
    size_t index;
    UmiStatus status;

    if (workspace == NULL || workspace_root == NULL ||
        out_selection == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_workspace_adapter_validate(workspace);
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_selection, 0, sizeof(*out_selection));
    umi_ai_coding_validation_plan_init(out_plan);

    for (index = 0U; index < sizeof(CANDIDATES) / sizeof(CANDIDATES[0]);
         ++index) {
        int exists = 0;

        status = workspace->exists(
            workspace->user_data,
            CANDIDATES[index].marker,
            &exists);
        if (status != UMI_STATUS_OK) return status;

        if (!exists) continue;

        status = CANDIDATES[index].factory(workspace_root, out_plan);
        if (status != UMI_STATUS_OK) return status;

        (void)snprintf(
            out_selection->profile_id,
            sizeof(out_selection->profile_id),
            "%s",
            CANDIDATES[index].profile_id);
        (void)snprintf(
            out_selection->marker_path,
            sizeof(out_selection->marker_path),
            "%s",
            CANDIDATES[index].marker);
        out_selection->detected = 1;
        return UMI_STATUS_OK;
    }

    return UMI_STATUS_NOT_FOUND;
}
