/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/toolchain_binding.h
 *
 * PURPOSE:
 *   Resolve language-aware build, run, debug, test and package tools without
 *   embedding vendor-specific decisions in Studio or another application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_TOOLCHAIN_BINDING_H
#define UMICOM_DEVELOPER_TOOLCHAIN_BINDING_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/language/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_TOOLCHAIN_BINDING_API_VERSION 1U
#define UMI_DEVELOPER_TOOLCHAIN_BINDING_CAPACITY 128U
#define UMI_DEVELOPER_TOOLCHAIN_COMMAND_CAPACITY 256U

/**
 * Represent the developer toolchain binding snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperToolchainBindingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char language_id[UMI_LANGUAGE_PROFILE_ID_CAPACITY];
    char platform[64];
    char architecture[64];
    char environment_profile[128];
    char compiler[UMI_DEVELOPER_TOOLCHAIN_COMMAND_CAPACITY];
    char linker[UMI_DEVELOPER_TOOLCHAIN_COMMAND_CAPACITY];
    char debugger[UMI_DEVELOPER_TOOLCHAIN_COMMAND_CAPACITY];
    char runtime[UMI_DEVELOPER_TOOLCHAIN_COMMAND_CAPACITY];
    char formatter[UMI_DEVELOPER_TOOLCHAIN_COMMAND_CAPACITY];
    char linter[UMI_DEVELOPER_TOOLCHAIN_COMMAND_CAPACITY];
    char test_runner[UMI_DEVELOPER_TOOLCHAIN_COMMAND_CAPACITY];
    char package_tool[UMI_DEVELOPER_TOOLCHAIN_COMMAND_CAPACITY];
    UmiLanguageCapabilityFlags capabilities;
    uint32_t priority;
    int enabled;
    uint64_t revision;
} UmiDeveloperToolchainBindingSnapshot;

/**
 * Represent the developer toolchain binding registry snapshot data shared with callers of
 * this public contract.
 */
typedef struct UmiDeveloperToolchainBindingRegistrySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t binding_count;
    size_t enabled_count;
    size_t build_capable_count;
    size_t debug_capable_count;
    size_t test_capable_count;
    size_t package_capable_count;
    uint64_t revision;
} UmiDeveloperToolchainBindingRegistrySnapshot;

/**
 * Represent the developer toolchain binding registry data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperToolchainBindingRegistry
    UmiDeveloperToolchainBindingRegistry;

/**
 * Initialise developer toolchain binding from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_toolchain_binding_init(
    UmiDeveloperToolchainBindingSnapshot *binding);
/**
 * Initialise developer toolchain binding registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_toolchain_binding_registry_create(
    UmiDeveloperToolchainBindingRegistry **out_registry);
/**
 * Release or reset state held by developer toolchain binding registry so the same storage
 * can be reused safely.
 */
void umi_developer_toolchain_binding_registry_destroy(
    UmiDeveloperToolchainBindingRegistry *registry);
/**
 * Provide the developer toolchain binding registry upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_developer_toolchain_binding_registry_upsert(
    UmiDeveloperToolchainBindingRegistry *registry,
    const UmiDeveloperToolchainBindingSnapshot *binding);
/**
 * Remove developer toolchain binding registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_developer_toolchain_binding_registry_remove(
    UmiDeveloperToolchainBindingRegistry *registry,
    const char *binding_id);
/**
 * Find developer toolchain binding registry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_developer_toolchain_binding_registry_find(
    const UmiDeveloperToolchainBindingRegistry *registry,
    const char *binding_id,
    UmiDeveloperToolchainBindingSnapshot *out_binding);
/**
 * Provide the developer toolchain binding registry resolve operation used by this module
 * and its client applications.
 */
UmiStatus umi_developer_toolchain_binding_registry_resolve(
    const UmiDeveloperToolchainBindingRegistry *registry,
    const char *language_id,
    const char *platform,
    const char *architecture,
    UmiLanguageCapabilityFlags required_capabilities,
    UmiDeveloperToolchainBindingSnapshot *out_binding);
/**
 * Find developer toolchain binding registry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_developer_toolchain_binding_registry_at(
    const UmiDeveloperToolchainBindingRegistry *registry,
    size_t index,
    UmiDeveloperToolchainBindingSnapshot *out_binding);
/**
 * Provide the developer toolchain binding registry snapshot operation used by this module
 * and its client applications.
 */
UmiStatus umi_developer_toolchain_binding_registry_snapshot(
    const UmiDeveloperToolchainBindingRegistry *registry,
    UmiDeveloperToolchainBindingRegistrySnapshot *out_snapshot);
/**
 * Provide the developer toolchain binding register builtins operation used by this module
 * and its client applications.
 */
UmiStatus umi_developer_toolchain_binding_register_builtins(
    UmiDeveloperToolchainBindingRegistry *registry);
/**
 * Return the number of records represented by developer toolchain binding builtin without
 * changing their state.
 */
size_t umi_developer_toolchain_binding_builtin_count(void);

#ifdef __cplusplus
}
#endif

#endif
