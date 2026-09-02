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

/**
 * List the named debug compound state values accepted by this public contract.
 */
typedef enum UmiDebugCompoundState {
    UMI_DEBUG_COMPOUND_EMPTY = 0,
    UMI_DEBUG_COMPOUND_CONFIGURING = 1,
    UMI_DEBUG_COMPOUND_READY = 2,
    UMI_DEBUG_COMPOUND_CANCELLED = 3
} UmiDebugCompoundState;

/**
 * List the named debug compound launch mode values accepted by this public contract.
 */
typedef enum UmiDebugCompoundLaunchMode {
    UMI_DEBUG_COMPOUND_LAUNCH_PARALLEL = 1,
    UMI_DEBUG_COMPOUND_LAUNCH_SEQUENTIAL = 2
} UmiDebugCompoundLaunchMode;

/**
 * Represent the debug compound descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugCompoundDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char label[256];
    UmiDebugCompoundLaunchMode launch_mode;
    int stop_all;
} UmiDebugCompoundDescriptor;

/**
 * Represent the debug compound entry data shared with callers of this public contract.
 */
typedef struct UmiDebugCompoundEntry {
    uint32_t struct_size;
    uint32_t api_version;
    char configuration_id[128];
    int32_t order;
    int enabled;
    int required;
    int launchable;
} UmiDebugCompoundEntry;

/**
 * Represent the debug compound snapshot data shared with callers of this public contract.
 */
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

/**
 * Represent the debug compound configuration data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugCompoundConfiguration UmiDebugCompoundConfiguration;

/**
 * Initialise debug compound configuration from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_compound_configuration_create(
    UmiDebugCompoundConfiguration **out_compound);
/**
 * Release or reset state held by debug compound configuration so the same storage can be
 * reused safely.
 */
void umi_debug_compound_configuration_destroy(
    UmiDebugCompoundConfiguration *compound);
/**
 * Provide the debug compound configuration begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_compound_configuration_begin(
    UmiDebugCompoundConfiguration *compound,
    const UmiDebugCompoundDescriptor *descriptor);
/**
 * Add debug compound configuration only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_debug_compound_configuration_add(
    UmiDebugCompoundConfiguration *compound,
    const UmiDebugCompoundEntry *entry);
/**
 * Remove debug compound configuration while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_debug_compound_configuration_remove(
    UmiDebugCompoundConfiguration *compound,
    const char *configuration_id);
/**
 * Provide the debug compound configuration set enabled operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_compound_configuration_set_enabled(
    UmiDebugCompoundConfiguration *compound,
    const char *configuration_id,
    int enabled);
/**
 * Provide the debug compound configuration finalize operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_compound_configuration_finalize(
    UmiDebugCompoundConfiguration *compound,
    const UmiDebugLaunchConfigurationRegistry *configurations,
    const UmiDebugAdapterProfileRegistry *adapters);
/**
 * Provide the debug compound configuration cancel operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_compound_configuration_cancel(
    UmiDebugCompoundConfiguration *compound);
/**
 * Find debug compound configuration while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_debug_compound_configuration_at(
    const UmiDebugCompoundConfiguration *compound,
    size_t index,
    UmiDebugCompoundEntry *out_entry);
/**
 * Provide the debug compound configuration snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_compound_configuration_snapshot(
    const UmiDebugCompoundConfiguration *compound,
    UmiDebugCompoundSnapshot *out_snapshot);
/**
 * Return the number of records represented by debug compound configuration without
 * changing their state.
 */
size_t umi_debug_compound_configuration_count(
    const UmiDebugCompoundConfiguration *compound);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_COMPOUND_CONFIGURATION_H */
