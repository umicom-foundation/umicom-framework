/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/launch_configuration.h
 *
 * PURPOSE:
 *   Define a DAP-friendly but adapter-neutral debugger record for native and future Umicom runtimes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/**
 * Represent the debug launch configuration snapshot data shared with callers of this
 * public contract.
 */
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

/**
 * Represent the debug launch configuration registry data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugLaunchConfigurationRegistry UmiDebugLaunchConfigurationRegistry;

/**
 * Initialise debug launch configuration registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_launch_configuration_registry_create(UmiDebugLaunchConfigurationRegistry **out_registry);
/**
 * Release or reset state held by debug launch configuration registry so the same storage
 * can be reused safely.
 */
void umi_debug_launch_configuration_registry_destroy(UmiDebugLaunchConfigurationRegistry *registry);
/**
 * Provide the debug launch configuration registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_launch_configuration_registry_upsert(UmiDebugLaunchConfigurationRegistry *registry, const UmiDebugLaunchConfigurationSnapshot *item);
/**
 * Remove debug launch configuration registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_debug_launch_configuration_registry_remove(UmiDebugLaunchConfigurationRegistry *registry, const char *id);
/**
 * Find debug launch configuration registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_debug_launch_configuration_registry_find(const UmiDebugLaunchConfigurationRegistry *registry, const char *id, UmiDebugLaunchConfigurationSnapshot *out_item);
/**
 * Find debug launch configuration registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_debug_launch_configuration_registry_at(const UmiDebugLaunchConfigurationRegistry *registry, size_t index, UmiDebugLaunchConfigurationSnapshot *out_item);
/**
 * Return the number of records represented by debug launch configuration registry without
 * changing their state.
 */
size_t umi_debug_launch_configuration_registry_count(const UmiDebugLaunchConfigurationRegistry *registry);
/**
 * Provide the debug launch configuration registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_debug_launch_configuration_registry_revision(const UmiDebugLaunchConfigurationRegistry *registry);
/**
 * Release or reset state held by debug launch configuration registry so the same storage
 * can be reused safely.
 */
void umi_debug_launch_configuration_registry_clear(UmiDebugLaunchConfigurationRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
