/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/configuration_resolver.h
 *
 * PURPOSE:
 *   Resolve reusable debugger launch configurations and adapter profiles for
 *   a workspace without placing substitution or validation logic in an app.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_CONFIGURATION_RESOLVER_H
#define UMICOM_DEBUG_CONFIGURATION_RESOLVER_H

#include "umicom/debug/adapter_profile.h"
#include "umicom/debug/launch_configuration.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_CONFIGURATION_RESOLVER_API_VERSION 1U
#define UMI_DEBUG_CONFIGURATION_VALUE_CAPACITY 2048U

/**
 * Represent the debug configuration request data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugConfigurationRequest {
    uint32_t struct_size;
    uint32_t api_version;
    char configuration_id[128];
    char workspace_folder[1024];
    char active_file[1024];
    char program_override[1024];
    char arguments_override[UMI_DEBUG_CONFIGURATION_VALUE_CAPACITY];
    char working_directory_override[1024];
    char environment_override[UMI_DEBUG_CONFIGURATION_VALUE_CAPACITY];
} UmiDebugConfigurationRequest;

/**
 * Represent the debug resolved configuration data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugResolvedConfiguration {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDebugLaunchConfigurationSnapshot launch;
    UmiDebugAdapterProfile adapter;
    size_t substituted_token_count;
    uint64_t revision;
    UmiStatus status;
    int ready;
    int uses_program_override;
    int uses_arguments_override;
    int uses_working_directory_override;
    int uses_environment_override;
} UmiDebugResolvedConfiguration;

/**
 * Represent the debug configuration resolver data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugConfigurationResolver UmiDebugConfigurationResolver;

/**
 * Initialise debug configuration resolver from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_configuration_resolver_create(
    UmiDebugConfigurationResolver **out_resolver);
/**
 * Release or reset state held by debug configuration resolver so the same storage can be
 * reused safely.
 */
void umi_debug_configuration_resolver_destroy(
    UmiDebugConfigurationResolver *resolver);
/**
 * Provide the debug configuration resolver resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_configuration_resolver_resolve(
    UmiDebugConfigurationResolver *resolver,
    const UmiDebugLaunchConfigurationRegistry *configurations,
    const UmiDebugAdapterProfileRegistry *adapters,
    const UmiDebugConfigurationRequest *request);
/**
 * Provide the debug configuration resolver result operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_configuration_resolver_result(
    const UmiDebugConfigurationResolver *resolver,
    UmiDebugResolvedConfiguration *out_result);
/**
 * Release or reset state held by debug configuration resolver so the same storage can be
 * reused safely.
 */
UmiStatus umi_debug_configuration_resolver_clear(
    UmiDebugConfigurationResolver *resolver);
/**
 * Provide the debug configuration resolver revision operation used by this module and its
 * client applications.
 */
uint64_t umi_debug_configuration_resolver_revision(
    const UmiDebugConfigurationResolver *resolver);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_CONFIGURATION_RESOLVER_H */
