/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/launch_configuration.h
 *
 * PURPOSE:
 *   Define a DAP-friendly but adapter-neutral debugger record for native and future Umicom runtimes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_DEBUG_LAUNCH_CONFIGURATION_H
#define UMICOM_DEBUG_LAUNCH_CONFIGURATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_LAUNCH_CONFIGURATION_CAPACITY 2048U
#define UMI_DEBUG_LAUNCH_CONFIGURATION_API_VERSION 1U

typedef struct UmiDebugLaunchConfigurationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char name[256];
    char adapter[128];
    char program[1024];
    char arguments[1024];
    char working_directory[1024];
    char environment[2048];
    int stop_on_entry;
    uint64_t revision;
} UmiDebugLaunchConfigurationSnapshot;

typedef struct UmiDebugLaunchConfigurationRegistry UmiDebugLaunchConfigurationRegistry;

UmiStatus umi_debug_launch_configuration_registry_create(UmiDebugLaunchConfigurationRegistry **out_registry);
void umi_debug_launch_configuration_registry_destroy(UmiDebugLaunchConfigurationRegistry *registry);
UmiStatus umi_debug_launch_configuration_registry_upsert(UmiDebugLaunchConfigurationRegistry *registry, const UmiDebugLaunchConfigurationSnapshot *item);
UmiStatus umi_debug_launch_configuration_registry_remove(UmiDebugLaunchConfigurationRegistry *registry, const char *id);
UmiStatus umi_debug_launch_configuration_registry_find(const UmiDebugLaunchConfigurationRegistry *registry, const char *id, UmiDebugLaunchConfigurationSnapshot *out_item);
UmiStatus umi_debug_launch_configuration_registry_at(const UmiDebugLaunchConfigurationRegistry *registry, size_t index, UmiDebugLaunchConfigurationSnapshot *out_item);
size_t umi_debug_launch_configuration_registry_count(const UmiDebugLaunchConfigurationRegistry *registry);
uint64_t umi_debug_launch_configuration_registry_revision(const UmiDebugLaunchConfigurationRegistry *registry);
void umi_debug_launch_configuration_registry_clear(UmiDebugLaunchConfigurationRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
