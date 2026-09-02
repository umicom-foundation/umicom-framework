/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/universal_model.h
 *
 * PURPOSE:
 *   Provide the Framework-owned source of truth that joins language profiles,
 *   project language matrices and toolchain bindings for polyglot development.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_UNIVERSAL_MODEL_H
#define UMICOM_DEVELOPER_UNIVERSAL_MODEL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/developer/toolchain_binding.h"
#include "umicom/project/language_matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_UNIVERSAL_MODEL_API_VERSION 1U
#define UMI_DEVELOPER_UNIVERSAL_PROJECT_CAPACITY 64U

/**
 * Represent the developer universal resolution snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperUniversalResolutionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char project_id[128];
    char source_path[1024];
    UmiLanguageProfileSnapshot language;
    UmiProjectLanguageEntrySnapshot project_language;
    UmiDeveloperToolchainBindingSnapshot toolchain;
    int has_project_language;
    int has_toolchain;
    uint64_t revision;
} UmiDeveloperUniversalResolutionSnapshot;

/**
 * Represent the developer universal project audit data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperUniversalProjectAudit {
    uint32_t struct_size;
    uint32_t api_version;
    char project_id[128];
    size_t enabled_language_count;
    size_t resolved_toolchain_count;
    size_t build_ready_count;
    size_t debug_ready_count;
    size_t test_ready_count;
    size_t package_ready_count;
    size_t unresolved_count;
    int primary_language_ready;
    char summary[512];
} UmiDeveloperUniversalProjectAudit;

/**
 * Represent the developer universal model snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperUniversalModelSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiLanguageProfileRegistrySnapshot languages;
    UmiDeveloperToolchainBindingRegistrySnapshot toolchains;
    size_t project_count;
    size_t configured_project_count;
    uint64_t revision;
} UmiDeveloperUniversalModelSnapshot;

/**
 * Represent the developer universal model data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperUniversalModel UmiDeveloperUniversalModel;

/**
 * Initialise developer universal model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_universal_model_create(
    UmiDeveloperUniversalModel **out_model);
/**
 * Release or reset state held by developer universal model so the same storage can be
 * reused safely.
 */
void umi_developer_universal_model_destroy(UmiDeveloperUniversalModel *model);
/**
 * Provide the developer universal model register project operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_universal_model_register_project(
    UmiDeveloperUniversalModel *model,
    const char *project_id,
    const char *primary_language_id);
/**
 * Provide the developer universal model configure language operation used by this module
 * and its client applications.
 */
UmiStatus umi_developer_universal_model_configure_language(
    UmiDeveloperUniversalModel *model,
    const char *project_id,
    const UmiProjectLanguageEntrySnapshot *entry);
/**
 * Provide the developer universal model remove project operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_universal_model_remove_project(
    UmiDeveloperUniversalModel *model,
    const char *project_id);
/**
 * Provide the developer universal model project matrix operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_universal_model_project_matrix(
    UmiDeveloperUniversalModel *model,
    const char *project_id,
    UmiProjectLanguageMatrix **out_matrix);
/**
 * Provide the developer universal model resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_universal_model_resolve(
    const UmiDeveloperUniversalModel *model,
    const char *project_id,
    const char *source_path,
    const char *platform,
    const char *architecture,
    UmiLanguageCapabilityFlags required_capabilities,
    UmiDeveloperUniversalResolutionSnapshot *out_resolution);
/**
 * Provide the developer universal model audit project operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_universal_model_audit_project(
    const UmiDeveloperUniversalModel *model,
    const char *project_id,
    const char *platform,
    const char *architecture,
    UmiDeveloperUniversalProjectAudit *out_audit);
/**
 * Provide the developer universal model snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_universal_model_snapshot(
    const UmiDeveloperUniversalModel *model,
    UmiDeveloperUniversalModelSnapshot *out_snapshot);
/**
 * Provide the developer universal model languages operation used by this module and its
 * client applications.
 */
UmiLanguageProfileRegistry *umi_developer_universal_model_languages(
    UmiDeveloperUniversalModel *model);
/**
 * Provide the developer universal model toolchains operation used by this module and its
 * client applications.
 */
UmiDeveloperToolchainBindingRegistry *
umi_developer_universal_model_toolchains(UmiDeveloperUniversalModel *model);

#ifdef __cplusplus
}
#endif

#endif
