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

/**
 * List the named ide surface region values accepted by this public contract.
 */
typedef enum UmiIdeSurfaceRegion {
    UMI_IDE_SURFACE_REGION_PRIMARY_SIDEBAR = 1,
    UMI_IDE_SURFACE_REGION_SECONDARY_SIDEBAR = 2,
    UMI_IDE_SURFACE_REGION_EDITOR = 3,
    UMI_IDE_SURFACE_REGION_BOTTOM_PANEL = 4,
    UMI_IDE_SURFACE_REGION_OVERLAY = 5
} UmiIdeSurfaceRegion;

/**
 * Represent the ide surface descriptor data shared with callers of this public contract.
 */
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

/**
 * Represent the ide surface registry data shared with callers of this public contract.
 */
typedef struct UmiIdeSurfaceRegistry UmiIdeSurfaceRegistry;

/**
 * Initialise ide surface registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ide_surface_registry_create(UmiIdeSurfaceRegistry **out_registry);
/**
 * Release or reset state held by ide surface registry so the same storage can be reused
 * safely.
 */
void umi_ide_surface_registry_destroy(UmiIdeSurfaceRegistry *registry);

/**
 * Add ide surface registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ide_surface_registry_add(
    UmiIdeSurfaceRegistry *registry,
    const UmiIdeSurfaceDescriptor *descriptor);

/**
 * Find ide surface registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiIdeSurfaceDescriptor *umi_ide_surface_registry_find(
    const UmiIdeSurfaceRegistry *registry,
    const char *surface_id);

/**
 * Find ide surface registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiIdeSurfaceDescriptor *umi_ide_surface_registry_at(
    const UmiIdeSurfaceRegistry *registry,
    size_t index);

/**
 * Return the number of records represented by ide surface registry without changing their
 * state.
 */
size_t umi_ide_surface_registry_count(
    const UmiIdeSurfaceRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
