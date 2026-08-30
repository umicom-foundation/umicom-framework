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

typedef struct UmiDeveloperUniversalModelSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiLanguageProfileRegistrySnapshot languages;
    UmiDeveloperToolchainBindingRegistrySnapshot toolchains;
    size_t project_count;
    size_t configured_project_count;
    uint64_t revision;
} UmiDeveloperUniversalModelSnapshot;

typedef struct UmiDeveloperUniversalModel UmiDeveloperUniversalModel;

UmiStatus umi_developer_universal_model_create(
    UmiDeveloperUniversalModel **out_model);
void umi_developer_universal_model_destroy(UmiDeveloperUniversalModel *model);
UmiStatus umi_developer_universal_model_register_project(
    UmiDeveloperUniversalModel *model,
    const char *project_id,
    const char *primary_language_id);
UmiStatus umi_developer_universal_model_configure_language(
    UmiDeveloperUniversalModel *model,
    const char *project_id,
    const UmiProjectLanguageEntrySnapshot *entry);
UmiStatus umi_developer_universal_model_remove_project(
    UmiDeveloperUniversalModel *model,
    const char *project_id);
UmiStatus umi_developer_universal_model_project_matrix(
    UmiDeveloperUniversalModel *model,
    const char *project_id,
    UmiProjectLanguageMatrix **out_matrix);
UmiStatus umi_developer_universal_model_resolve(
    const UmiDeveloperUniversalModel *model,
    const char *project_id,
    const char *source_path,
    const char *platform,
    const char *architecture,
    UmiLanguageCapabilityFlags required_capabilities,
    UmiDeveloperUniversalResolutionSnapshot *out_resolution);
UmiStatus umi_developer_universal_model_audit_project(
    const UmiDeveloperUniversalModel *model,
    const char *project_id,
    const char *platform,
    const char *architecture,
    UmiDeveloperUniversalProjectAudit *out_audit);
UmiStatus umi_developer_universal_model_snapshot(
    const UmiDeveloperUniversalModel *model,
    UmiDeveloperUniversalModelSnapshot *out_snapshot);
UmiLanguageProfileRegistry *umi_developer_universal_model_languages(
    UmiDeveloperUniversalModel *model);
UmiDeveloperToolchainBindingRegistry *
umi_developer_universal_model_toolchains(UmiDeveloperUniversalModel *model);

#ifdef __cplusplus
}
#endif

#endif
