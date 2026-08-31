/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_binding.c
 *
 * PURPOSE:
 *   Implement compatibility-safe IDE-surface to Application Shell binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_binding.h"

#include <string.h>

static UmiStatus find_compatible(
    UmiApplicationShellRegistry *registry,
    const UmiStudioRuntimeSurfaceBinding *binding,
    UmiApplicationShellContribution *out_contribution)
{
    size_t index;

    for (index = 0U;
         index < umi_application_shell_registry_count(registry);
         ++index) {
        UmiApplicationShellContribution candidate;

        if (umi_application_shell_registry_at(
                registry,
                index,
                &candidate) != UMI_STATUS_OK) {
            continue;
        }

        if (candidate.role == binding->role &&
            candidate.region == binding->region &&
            strcmp(candidate.title, binding->title) == 0) {
            *out_contribution = candidate;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_studio_runtime_surface_binding_validate(
    const UmiStudioRuntimeSurfaceBinding *binding)
{
    if (binding == NULL ||
        binding->kind < UMI_STUDIO_SURFACE_EXPLORER ||
        binding->kind > UMI_STUDIO_SURFACE_LAST ||
        binding->ide_surface_id == NULL ||
        binding->ide_surface_id[0] == '\0' ||
        binding->fallback_contribution_id == NULL ||
        binding->fallback_contribution_id[0] == '\0' ||
        binding->title == NULL ||
        binding->title[0] == '\0' ||
        binding->role < UMI_APPLICATION_SHELL_ROLE_MENU ||
        binding->role > UMI_APPLICATION_SHELL_ROLE_SERVICE_SURFACE ||
        binding->region < UMI_APPLICATION_SHELL_REGION_NONE ||
        binding->region > UMI_APPLICATION_SHELL_REGION_WINDOW) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_studio_runtime_surface_resolve(
    UmiApplicationShellRegistry *registry,
    const UmiStudioRuntimeSurfaceBinding *binding,
    UmiApplicationShellContribution *out_contribution)
{
    UmiStatus status;

    if (registry == NULL || out_contribution == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_surface_binding_validate(binding);
    if (status != UMI_STATUS_OK) return status;

    if (binding->preferred_contribution_id != NULL &&
        binding->preferred_contribution_id[0] != '\0') {
        status = umi_application_shell_registry_find(
            registry,
            binding->preferred_contribution_id,
            out_contribution);
        if (status == UMI_STATUS_OK) return UMI_STATUS_OK;
    }

    status = find_compatible(registry, binding, out_contribution);
    if (status == UMI_STATUS_OK) return UMI_STATUS_OK;

    return umi_application_shell_registry_find(
        registry,
        binding->fallback_contribution_id,
        out_contribution);
}

UmiStatus umi_studio_runtime_surface_install(
    UmiApplicationShellRegistry *registry,
    UmiApplicationShellLayout *layout,
    const UmiStudioRuntimeSurfaceBinding *binding)
{
    UmiApplicationShellContribution contribution;
    UmiStatus status;

    if (registry == NULL || layout == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_surface_resolve(
        registry,
        binding,
        &contribution);

    if (status == UMI_STATUS_NOT_FOUND && binding->create_if_missing) {
        umi_application_shell_contribution_init(
            &contribution,
            binding->fallback_contribution_id,
            binding->title,
            binding->role,
            binding->region);

        contribution.flags =
            UMI_APPLICATION_SHELL_ENABLED |
            UMI_APPLICATION_SHELL_MOVABLE |
            UMI_APPLICATION_SHELL_CLOSABLE |
            UMI_APPLICATION_SHELL_CONTEXT_AWARE;

        if (binding->default_visible) {
            contribution.flags |= UMI_APPLICATION_SHELL_VISIBLE;
        }

        contribution.order = binding->order;

        status = umi_application_shell_registry_upsert(
            registry,
            &contribution);
    }

    if (status != UMI_STATUS_OK) return status;

    return umi_application_shell_layout_place(
        layout,
        contribution.contribution_id,
        binding->container_id != NULL ? binding->container_id : "",
        binding->region,
        (size_t)(binding->order >= 0 ? binding->order : 0),
        binding->default_visible);
}
