/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/catalogue.h
 *
 * PURPOSE:
 *   Manage several discovered compiler profiles and deterministic per-project
 *   selections. The catalogue is bounded, toolkit-neutral and safe to share
 *   between Studio, command-line tools and future Umicom products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_CATALOGUE_H
#define UMICOM_TOOLCHAIN_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/toolchain/capability.h"
#include "umicom/toolchain/discovery.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOLCHAIN_CATALOGUE_API_VERSION 1U
#define UMI_TOOLCHAIN_CATALOGUE_CAPACITY 16U
#define UMI_TOOLCHAIN_PROJECT_BINDING_CAPACITY 256U
#define UMI_TOOLCHAIN_DISCOVERY_ROOT_CAPACITY 8U

/**
 * Represent the toolchain catalogue data shared with callers of this public contract.
 */
typedef struct UmiToolchainCatalogue UmiToolchainCatalogue;

/**
 * Represent the toolchain project binding snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiToolchainProjectBindingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char project_id[128];
    char profile_id[UMI_TOOLCHAIN_TEXT_CAPACITY];
    char compilation_database[UMI_TOOL_PATH_CAPACITY];
    uint64_t revision;
} UmiToolchainProjectBindingSnapshot;

/**
 * Represent the toolchain catalogue snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiToolchainCatalogueSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t revision;
    size_t profile_count;
    size_t complete_profile_count;
    size_t c23_profile_count;
    size_t project_binding_count;
    char selected_profile_id[UMI_TOOLCHAIN_TEXT_CAPACITY];
    int has_selected_profile;
} UmiToolchainCatalogueSnapshot;

/**
 * Represent the toolchain catalogue discovery request data shared with callers of this
 * public contract.
 */
typedef struct UmiToolchainCatalogueDiscoveryRequest {
    const char *const *explicit_roots;
    size_t explicit_root_count;
    const char *preferred_profile;
    int require_c23;
    int require_gtk;
    int require_java;
    int require_github_cli;
    int run_compile_probe;
    UmiDiagnosticSink diagnostic_sink;
    void *diagnostic_user_data;
} UmiToolchainCatalogueDiscoveryRequest;

/**
 * Represent the toolchain catalogue discovery snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiToolchainCatalogueDiscoverySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t attempted_profile_count;
    size_t discovered_profile_count;
    size_t rejected_profile_count;
    size_t complete_profile_count;
    size_t c23_profile_count;
    char selected_profile_id[UMI_TOOLCHAIN_TEXT_CAPACITY];
} UmiToolchainCatalogueDiscoverySnapshot;

/**
 * Initialise toolchain catalogue from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_toolchain_catalogue_create(UmiToolchainCatalogue **out_catalogue);
/**
 * Release or reset state held by toolchain catalogue so the same storage can be reused
 * safely.
 */
void umi_toolchain_catalogue_destroy(UmiToolchainCatalogue *catalogue);
/**
 * Provide the toolchain catalogue snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_catalogue_snapshot(
    const UmiToolchainCatalogue *catalogue,
    UmiToolchainCatalogueSnapshot *out_snapshot);
/**
 * Provide the toolchain catalogue upsert profile operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_catalogue_upsert_profile(
    UmiToolchainCatalogue *catalogue,
    const UmiToolchainProfile *profile);
/**
 * Provide the toolchain catalogue find profile operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_catalogue_find_profile(
    const UmiToolchainCatalogue *catalogue,
    const char *profile_id,
    UmiToolchainProfile *out_profile);
/**
 * Find toolchain catalogue profile while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_toolchain_catalogue_profile_at(
    const UmiToolchainCatalogue *catalogue,
    size_t index,
    UmiToolchainProfile *out_profile);
/**
 * Return the number of records represented by toolchain catalogue profile without changing
 * their state.
 */
size_t umi_toolchain_catalogue_profile_count(
    const UmiToolchainCatalogue *catalogue);
/**
 * Provide the toolchain catalogue select profile operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_catalogue_select_profile(
    UmiToolchainCatalogue *catalogue,
    const char *profile_id);
/**
 * Provide the toolchain catalogue resolve selected profile operation used by this module
 * and its client applications.
 */
UmiStatus umi_toolchain_catalogue_resolve_selected_profile(
    const UmiToolchainCatalogue *catalogue,
    int require_c23,
    UmiToolchainProfile *out_profile);
/**
 * Provide the toolchain catalogue bind project operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_catalogue_bind_project(
    UmiToolchainCatalogue *catalogue,
    const UmiToolchainProjectBindingSnapshot *binding);
/**
 * Provide the toolchain catalogue resolve project profile operation used by this module
 * and its client applications.
 */
UmiStatus umi_toolchain_catalogue_resolve_project_profile(
    const UmiToolchainCatalogue *catalogue,
    const char *project_id,
    int require_c23,
    UmiToolchainProfile *out_profile);
/**
 * Find toolchain catalogue project binding while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_toolchain_catalogue_project_binding_at(
    const UmiToolchainCatalogue *catalogue,
    size_t index,
    UmiToolchainProjectBindingSnapshot *out_binding);
/**
 * Provide the toolchain catalogue discover operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_catalogue_discover(
    UmiToolchainCatalogue *catalogue,
    const UmiToolchainCatalogueDiscoveryRequest *request,
    UmiToolchainCatalogueDiscoverySnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
