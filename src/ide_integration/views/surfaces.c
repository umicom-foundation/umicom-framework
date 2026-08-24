/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/views/surfaces.c
 *
 * PURPOSE:
 *   Project logical IDE surfaces supplied by the Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/views/surfaces.h"

#include <stdio.h>

UmiStatus umi_ide_surfaces_view_create(
    const char *view_id,
    UmiIdeIntegrationPlatform *platform,
    UmiUiViewModel **out_view)
{
    UmiIdeSurfaceRegistry *registry;
    size_t count;
    size_t index;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    registry = umi_ide_integration_platform_surfaces(platform);
    if (registry == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_ide_view_create_base(
        view_id,
        "ide-surfaces",
        "IDE Surfaces",
        "Logical Framework surfaces available for workbench docking and composition.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    count = umi_ide_surface_registry_count(registry);

    status = umi_ide_view_set_integer(
        *out_view, "ide.surfaces.count", (int64_t)count);

    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        const UmiIdeSurfaceDescriptor *surface =
            umi_ide_surface_registry_at(registry, index);
        char key[96];
        char text[512];

        if (surface == NULL) continue;

        (void)snprintf(key, sizeof(key), "ide.surfaces.row.%zu", index);
        (void)snprintf(
            text, sizeof(text),
            "%.120s | %.190s | region=%u | visible=%d | movable=%d",
            surface->title,
            surface->surface_id,
            (unsigned)surface->default_region,
            surface->default_visible,
            surface->movable);

        status = umi_ide_view_set_string(*out_view, key, text);
    }

    return status;
}
