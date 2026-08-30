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

typedef struct UmiDebugConfigurationResolver UmiDebugConfigurationResolver;

UmiStatus umi_debug_configuration_resolver_create(
    UmiDebugConfigurationResolver **out_resolver);
void umi_debug_configuration_resolver_destroy(
    UmiDebugConfigurationResolver *resolver);
UmiStatus umi_debug_configuration_resolver_resolve(
    UmiDebugConfigurationResolver *resolver,
    const UmiDebugLaunchConfigurationRegistry *configurations,
    const UmiDebugAdapterProfileRegistry *adapters,
    const UmiDebugConfigurationRequest *request);
UmiStatus umi_debug_configuration_resolver_result(
    const UmiDebugConfigurationResolver *resolver,
    UmiDebugResolvedConfiguration *out_result);
UmiStatus umi_debug_configuration_resolver_clear(
    UmiDebugConfigurationResolver *resolver);
uint64_t umi_debug_configuration_resolver_revision(
    const UmiDebugConfigurationResolver *resolver);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEBUG_CONFIGURATION_RESOLVER_H */
