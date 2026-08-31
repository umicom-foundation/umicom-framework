/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_catalogue.c
 *
 * PURPOSE:
 *   Implement the complete Studio surface binding catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_catalogue.h"

#include <string.h>

typedef const UmiStudioRuntimeSurfaceBinding *(*SurfaceFactory)(void);

static const SurfaceFactory FACTORIES[] = {
    umi_studio_surface_profile_explorer,
    umi_studio_surface_profile_editor,
    umi_studio_surface_profile_outline,
    umi_studio_surface_profile_search,
    umi_studio_surface_profile_problems,
    umi_studio_surface_profile_output,
    umi_studio_surface_profile_terminal,
    umi_studio_surface_profile_source_control,
    umi_studio_surface_profile_test_explorer,
    umi_studio_surface_profile_debug,
    umi_studio_surface_profile_build,
    umi_studio_surface_profile_ai_chat,
    umi_studio_surface_profile_ai_review,
    umi_studio_surface_profile_ai_approvals,
    umi_studio_surface_profile_ai_tools,
    umi_studio_surface_profile_ai_validation,
    umi_studio_surface_profile_ai_model_comparison
};

size_t umi_studio_surface_catalogue_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_catalogue_at(size_t index)
{
    return index < umi_studio_surface_catalogue_count()
        ? FACTORIES[index]()
        : NULL;
}

const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_catalogue_for_kind(UmiStudioRuntimeSurfaceKind kind)
{
    size_t index;

    for (index = 0U; index < umi_studio_surface_catalogue_count(); ++index) {
        const UmiStudioRuntimeSurfaceBinding *binding = FACTORIES[index]();

        if (binding != NULL && binding->kind == kind) return binding;
    }

    return NULL;
}

const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_catalogue_find_ide_surface(const char *surface_id)
{
    size_t index;

    if (surface_id == NULL) return NULL;

    for (index = 0U; index < umi_studio_surface_catalogue_count(); ++index) {
        const UmiStudioRuntimeSurfaceBinding *binding = FACTORIES[index]();

        if (binding != NULL &&
            strcmp(binding->ide_surface_id, surface_id) == 0) {
            return binding;
        }
    }

    return NULL;
}

UmiStatus umi_studio_surface_catalogue_install(
    UmiApplicationShellRegistry *registry,
    UmiApplicationShellLayout *layout)
{
    size_t index;

    if (registry == NULL || layout == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < umi_studio_surface_catalogue_count(); ++index) {
        UmiStatus status = umi_studio_runtime_surface_install(
            registry,
            layout,
            FACTORIES[index]());

        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
