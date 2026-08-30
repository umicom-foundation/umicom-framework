/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/surface.h
 *
 * PURPOSE:
 *   Describe logical IDE surfaces that compose existing Framework capabilities.
 *   Physical docking/geometry remains owned by the workbench/layout engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_SURFACE_H
#define UMICOM_IDE_INTEGRATION_SURFACE_H

#include "umicom/ide_integration/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_IDE_SURFACE_CAPACITY 32U

typedef enum UmiIdeSurfaceRegion {
    UMI_IDE_SURFACE_REGION_PRIMARY_SIDEBAR = 1,
    UMI_IDE_SURFACE_REGION_SECONDARY_SIDEBAR = 2,
    UMI_IDE_SURFACE_REGION_EDITOR = 3,
    UMI_IDE_SURFACE_REGION_BOTTOM_PANEL = 4,
    UMI_IDE_SURFACE_REGION_OVERLAY = 5
} UmiIdeSurfaceRegion;

typedef struct UmiIdeSurfaceDescriptor {
    char surface_id[UMI_IDE_INTEGRATION_ID_CAPACITY];
    char title[256];
    char description[512];
    char command_id[UMI_IDE_INTEGRATION_ID_CAPACITY];
    char required_capability[UMI_IDE_INTEGRATION_ID_CAPACITY];
    UmiIdeSurfaceRegion default_region;
    int default_visible;
    int closable;
    int movable;
    int multi_instance;
    uint64_t revision;
} UmiIdeSurfaceDescriptor;

typedef struct UmiIdeSurfaceRegistry UmiIdeSurfaceRegistry;

UmiStatus umi_ide_surface_registry_create(UmiIdeSurfaceRegistry **out_registry);
void umi_ide_surface_registry_destroy(UmiIdeSurfaceRegistry *registry);

UmiStatus umi_ide_surface_registry_add(
    UmiIdeSurfaceRegistry *registry,
    const UmiIdeSurfaceDescriptor *descriptor);

const UmiIdeSurfaceDescriptor *umi_ide_surface_registry_find(
    const UmiIdeSurfaceRegistry *registry,
    const char *surface_id);

const UmiIdeSurfaceDescriptor *umi_ide_surface_registry_at(
    const UmiIdeSurfaceRegistry *registry,
    size_t index);

size_t umi_ide_surface_registry_count(
    const UmiIdeSurfaceRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
