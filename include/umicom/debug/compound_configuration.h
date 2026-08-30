/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/compound_configuration.h
 *
 * PURPOSE:
 *   Build deterministic multi-configuration debug plans for applications that
 *   launch related processes, services or runtimes together.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_COMPOUND_CONFIGURATION_H
#define UMICOM_DEBUG_COMPOUND_CONFIGURATION_H

#include "umicom/debug/configuration_resolver.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_COMPOUND_CONFIGURATION_API_VERSION 1U

typedef enum UmiDebugCompoundState {
    UMI_DEBUG_COMPOUND_EMPTY = 0,
    UMI_DEBUG_COMPOUND_CONFIGURING = 1,
    UMI_DEBUG_COMPOUND_READY = 2,
    UMI_DEBUG_COMPOUND_CANCELLED = 3
} UmiDebugCompoundState;

typedef enum UmiDebugCompoundLaunchMode {
    UMI_DEBUG_COMPOUND_LAUNCH_PARALLEL = 1,
    UMI_DEBUG_COMPOUND_LAUNCH_SEQUENTIAL = 2
} UmiDebugCompoundLaunchMode;

typedef struct UmiDebugCompoundDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char label[256];
    UmiDebugCompoundLaunchMode launch_mode;
    int stop_all;
} UmiDebugCompoundDescriptor;

typedef struct UmiDebugCompoundEntry {
    uint32_t struct_size;
    uint32_t api_version;
    char configuration_id[128];
    int32_t order;
    int enabled;
    int required;
    int launchable;
} UmiDebugCompoundEntry;

typedef struct UmiDebugCompoundSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDebugCompoundState state;
    UmiDebugCompoundLaunchMode launch_mode;
    size_t entry_count;
    size_t enabled_count;
    size_t launchable_count;
    size_t required_count;
    uint64_t revision;
    int stop_all;
} UmiDebugCompoundSnapshot;

typedef struct UmiDebugCompoundConfiguration UmiDebugCompoundConfiguration;

UmiStatus umi_debug_compound_configuration_create(
    UmiDebugCompoundConfiguration **out_compound);
void umi_debug_compound_configuration_destroy(
    UmiDebugCompoundConfiguration *compound);
UmiStatus umi_debug_compound_configuration_begin(
    UmiDebugCompoundConfiguration *compound,
    const UmiDebugCompoundDescriptor *descriptor);
UmiStatus umi_debug_compound_configuration_add(
    UmiDebugCompoundConfiguration *compound,
    const UmiDebugCompoundEntry *entry);
UmiStatus umi_debug_compound_configuration_remove(
    UmiDebugCompoundConfiguration *compound,
    const char *configuration_id);
UmiStatus umi_debug_compound_configuration_set_enabled(
    UmiDebugCompoundConfiguration *compound,
    const char *configuration_id,
    int enabled);
UmiStatus umi_debug_compound_configuration_finalize(
    UmiDebugCompoundConfiguration *compound,
    const UmiDebugLaunchConfigurationRegistry *configurations,
    const UmiDebugAdapterProfileRegistry *adapters);
UmiStatus umi_debug_compound_configuration_cancel(
    UmiDebugCompoundConfiguration *compound);
UmiStatus umi_debug_compound_configuration_at(
    const UmiDebugCompoundConfiguration *compound,
    size_t index,
    UmiDebugCompoundEntry *out_entry);
UmiStatus umi_debug_compound_configuration_snapshot(
    const UmiDebugCompoundConfiguration *compound,
    UmiDebugCompoundSnapshot *out_snapshot);
size_t umi_debug_compound_configuration_count(
    const UmiDebugCompoundConfiguration *compound);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_COMPOUND_CONFIGURATION_H */
