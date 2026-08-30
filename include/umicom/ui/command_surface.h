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

/* BEGINNER NOTE:
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

typedef struct UmiUiCommandSurfaceRegistry UmiUiCommandSurfaceRegistry;

UmiStatus umi_ui_command_surface_registry_create(UmiUiCommandSurfaceRegistry **out_registry);
void umi_ui_command_surface_registry_destroy(UmiUiCommandSurfaceRegistry *registry);
UmiStatus umi_ui_command_surface_registry_upsert(UmiUiCommandSurfaceRegistry *registry, const UmiUiCommandSurfaceSnapshot *item);
UmiStatus umi_ui_command_surface_registry_remove(UmiUiCommandSurfaceRegistry *registry, const char *id);
UmiStatus umi_ui_command_surface_registry_find(const UmiUiCommandSurfaceRegistry *registry, const char *id, UmiUiCommandSurfaceSnapshot *out_item);
UmiStatus umi_ui_command_surface_registry_at(const UmiUiCommandSurfaceRegistry *registry, size_t index, UmiUiCommandSurfaceSnapshot *out_item);
size_t umi_ui_command_surface_registry_count(const UmiUiCommandSurfaceRegistry *registry);
uint64_t umi_ui_command_surface_registry_revision(const UmiUiCommandSurfaceRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
