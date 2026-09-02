/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/command_surface.h
 *
 * PURPOSE:
 *   Define command-palette and command-surface presentation records independent of a toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_UI_COMMAND_SURFACE_H
#define UMICOM_UI_COMMAND_SURFACE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_COMMAND_SURFACE_CAPACITY 2048U

/**
 * Represent the ui command surface snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiUiCommandSurfaceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char command_id[128];
    char title[256];
    char category[128];
    char icon_name[128];
    char key_hint[128];
    char when_expression[256];
    int enabled;
    int visible;
    int32_t score;
    uint64_t revision;
} UmiUiCommandSurfaceSnapshot;

/**
 * Represent the ui command surface registry data shared with callers of this public
 * contract.
 */
typedef struct UmiUiCommandSurfaceRegistry UmiUiCommandSurfaceRegistry;

/**
 * Initialise ui command surface registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_command_surface_registry_create(UmiUiCommandSurfaceRegistry **out_registry);
/**
 * Release or reset state held by ui command surface registry so the same storage can be
 * reused safely.
 */
void umi_ui_command_surface_registry_destroy(UmiUiCommandSurfaceRegistry *registry);
/**
 * Provide the ui command surface registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_command_surface_registry_upsert(UmiUiCommandSurfaceRegistry *registry, const UmiUiCommandSurfaceSnapshot *item);
/**
 * Remove ui command surface registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_command_surface_registry_remove(UmiUiCommandSurfaceRegistry *registry, const char *id);
/**
 * Find ui command surface registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_command_surface_registry_find(const UmiUiCommandSurfaceRegistry *registry, const char *id, UmiUiCommandSurfaceSnapshot *out_item);
/**
 * Find ui command surface registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_command_surface_registry_at(const UmiUiCommandSurfaceRegistry *registry, size_t index, UmiUiCommandSurfaceSnapshot *out_item);
/**
 * Return the number of records represented by ui command surface registry without changing
 * their state.
 */
size_t umi_ui_command_surface_registry_count(const UmiUiCommandSurfaceRegistry *registry);
/**
 * Provide the ui command surface registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ui_command_surface_registry_revision(const UmiUiCommandSurfaceRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
