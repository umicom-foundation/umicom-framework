/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/universal_model.c
 * PURPOSE: Join universal language, project and toolchain contracts.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/universal_model.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDeveloperUniversalModel {
    UmiLanguageProfileRegistry *languages;
    UmiDeveloperToolchainBindingRegistry *toolchains;
    UmiProjectLanguageMatrix *projects[UMI_DEVELOPER_UNIVERSAL_PROJECT_CAPACITY];
    size_t project_count;
    uint64_t revision;
};

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
}

/*
 * Provide the find project index operation used by this module and its client
 * applications.
 */
static size_t find_project_index(const UmiDeveloperUniversalModel *model,
                                 const char *project_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->project_count; ++index) {
        UmiProjectLanguageMatrixSnapshot snapshot;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_project_language_matrix_snapshot(model->projects[index], &snapshot) ==
                UMI_STATUS_OK && strcmp(snapshot.project_id, project_id) == 0)
            return index;
    }
    return SIZE_MAX;
}

/*
 * Initialise developer universal model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_universal_model_create(
    UmiDeveloperUniversalModel **out_model)
{
    UmiDeveloperUniversalModel *model;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiDeveloperUniversalModel *)calloc(1U, sizeof(*model));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->revision = 1U;
    status = umi_language_profile_registry_create(&model->languages);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_language_profile_register_builtins(model->languages);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_developer_toolchain_binding_registry_create(&model->toolchains);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_developer_toolchain_binding_register_builtins(model->toolchains);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_developer_universal_model_destroy(model);
        return status;
    }
    *out_model = model;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by developer universal model so the same storage can be
 * reused safely.
 */
void umi_developer_universal_model_destroy(UmiDeveloperUniversalModel *model)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->project_count; ++index)
        umi_project_language_matrix_destroy(model->projects[index]);
    umi_developer_toolchain_binding_registry_destroy(model->toolchains);
    umi_language_profile_registry_destroy(model->languages);
    free(model);
}

/*
 * Provide the developer universal model register project operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_universal_model_register_project(
    UmiDeveloperUniversalModel *model, const char *project_id,
    const char *primary_language_id)
{
    size_t index;
    UmiProjectLanguageMatrix *matrix;
    UmiProjectLanguageEntrySnapshot entry;
    UmiLanguageProfileSnapshot profile;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || project_id == NULL || project_id[0] == '\0' ||
        primary_language_id == NULL || primary_language_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_language_profile_registry_find(
        model->languages, primary_language_id, &profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    index = find_project_index(model, project_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->project_count >= UMI_DEVELOPER_UNIVERSAL_PROJECT_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        status = umi_project_language_matrix_create(project_id, &matrix);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        index = model->project_count++;
        model->projects[index] = matrix;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        matrix = model->projects[index];
    }
    status = umi_project_language_matrix_set_primary(matrix, primary_language_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_project_language_matrix_find(matrix, primary_language_id, &entry) ==
        UMI_STATUS_NOT_FOUND) {
        umi_project_language_entry_init(&entry);
        copy_text(entry.language_id, sizeof(entry.language_id), primary_language_id);
        copy_text(entry.source_roots, sizeof(entry.source_roots), "src");
        copy_text(entry.standard, sizeof(entry.standard), profile.default_standard);
        entry.compile_order = 0U;
        status = umi_project_language_matrix_upsert(matrix, &entry);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer universal model configure language operation used by this module
 * and its client applications.
 */
UmiStatus umi_developer_universal_model_configure_language(
    UmiDeveloperUniversalModel *model, const char *project_id,
    const UmiProjectLanguageEntrySnapshot *entry)
{
    size_t index;
    UmiLanguageProfileSnapshot profile;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || project_id == NULL || entry == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_project_index(model, project_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    status = umi_language_profile_registry_find(
        model->languages, entry->language_id, &profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_project_language_matrix_upsert(model->projects[index], entry);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) model->revision += 1U;
    return status;
}

/*
 * Provide the developer universal model remove project operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_universal_model_remove_project(
    UmiDeveloperUniversalModel *model, const char *project_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || project_id == NULL || project_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_project_index(model, project_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    umi_project_language_matrix_destroy(model->projects[index]);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < model->project_count) {
        memmove(&model->projects[index], &model->projects[index + 1U],
                (model->project_count - index - 1U) * sizeof(model->projects[0]));
    }
    model->project_count -= 1U;
    model->projects[model->project_count] = NULL;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer universal model project matrix operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_universal_model_project_matrix(
    UmiDeveloperUniversalModel *model, const char *project_id,
    UmiProjectLanguageMatrix **out_matrix)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || project_id == NULL || out_matrix == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_matrix = NULL;
    index = find_project_index(model, project_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_matrix = model->projects[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the developer universal model resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_universal_model_resolve(
    const UmiDeveloperUniversalModel *model, const char *project_id,
    const char *source_path, const char *platform, const char *architecture,
    UmiLanguageCapabilityFlags required_capabilities,
    UmiDeveloperUniversalResolutionSnapshot *out_resolution)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || project_id == NULL || source_path == NULL ||
        out_resolution == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_resolution, 0, sizeof(*out_resolution));
    out_resolution->struct_size = (uint32_t)sizeof(*out_resolution);
    out_resolution->api_version = UMI_DEVELOPER_UNIVERSAL_MODEL_API_VERSION;
    copy_text(out_resolution->project_id, sizeof(out_resolution->project_id), project_id);
    copy_text(out_resolution->source_path, sizeof(out_resolution->source_path), source_path);
    index = find_project_index(model, project_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    status = umi_language_profile_registry_find_for_path(
        model->languages, source_path, &out_resolution->language);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_language_profile_supports(&out_resolution->language,
                                       required_capabilities))
        return UMI_STATUS_UNAVAILABLE;
    status = umi_project_language_matrix_find(
        model->projects[index], out_resolution->language.id,
        &out_resolution->project_language);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || out_resolution->project_language.enabled == 0)
        return UMI_STATUS_UNAVAILABLE;
    out_resolution->has_project_language = 1;
    status = umi_developer_toolchain_binding_registry_resolve(
        model->toolchains, out_resolution->language.id, platform, architecture,
        required_capabilities, &out_resolution->toolchain);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) out_resolution->has_toolchain = 1;
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (required_capabilities != 0U) return status;
    out_resolution->revision = model->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer universal model audit project operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_universal_model_audit_project(
    const UmiDeveloperUniversalModel *model, const char *project_id,
    const char *platform, const char *architecture,
    UmiDeveloperUniversalProjectAudit *out_audit)
{
    size_t project_index;
    size_t language_index;
    UmiProjectLanguageMatrixSnapshot matrix_snapshot;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || project_id == NULL || out_audit == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    project_index = find_project_index(model, project_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (project_index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    memset(out_audit, 0, sizeof(*out_audit));
    out_audit->struct_size = (uint32_t)sizeof(*out_audit);
    out_audit->api_version = UMI_DEVELOPER_UNIVERSAL_MODEL_API_VERSION;
    copy_text(out_audit->project_id, sizeof(out_audit->project_id), project_id);
    (void)umi_project_language_matrix_snapshot(
        model->projects[project_index], &matrix_snapshot);
    /* Visit each bounded item once so every record receives the same rule. */
    for (language_index = 0U;
         language_index < umi_project_language_matrix_count(model->projects[project_index]);
         ++language_index) {
        UmiProjectLanguageEntrySnapshot entry;
        UmiDeveloperToolchainBindingSnapshot binding;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_project_language_matrix_at(model->projects[project_index],
                                           language_index, &entry) != UMI_STATUS_OK ||
            entry.enabled == 0) continue;
        out_audit->enabled_language_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_developer_toolchain_binding_registry_resolve(
                model->toolchains, entry.language_id, platform, architecture, 0U,
                &binding) != UMI_STATUS_OK) {
            out_audit->unresolved_count += 1U;
            continue;
        }
        out_audit->resolved_toolchain_count += 1U;
        /* Create this optional product surface only when its build option is enabled. */
        if ((binding.capabilities & UMI_LANGUAGE_CAPABILITY_BUILD) != 0U)
            out_audit->build_ready_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if ((binding.capabilities & UMI_LANGUAGE_CAPABILITY_DEBUG) != 0U)
            out_audit->debug_ready_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if ((binding.capabilities & UMI_LANGUAGE_CAPABILITY_TEST) != 0U)
            out_audit->test_ready_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if ((binding.capabilities & UMI_LANGUAGE_CAPABILITY_PACKAGE) != 0U)
            out_audit->package_ready_count += 1U;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(entry.language_id, matrix_snapshot.primary_language_id) == 0)
            out_audit->primary_language_ready = 1;
    }
    (void)snprintf(out_audit->summary, sizeof(out_audit->summary),
                   "%zu enabled language(s), %zu resolved toolchain(s), %zu unresolved",
                   out_audit->enabled_language_count,
                   out_audit->resolved_toolchain_count,
                   out_audit->unresolved_count);
    return UMI_STATUS_OK;
}

/*
 * Provide the developer universal model snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_universal_model_snapshot(
    const UmiDeveloperUniversalModel *model,
    UmiDeveloperUniversalModelSnapshot *out_snapshot)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEVELOPER_UNIVERSAL_MODEL_API_VERSION;
    (void)umi_language_profile_registry_snapshot(model->languages,
                                                 &out_snapshot->languages);
    (void)umi_developer_toolchain_binding_registry_snapshot(
        model->toolchains, &out_snapshot->toolchains);
    out_snapshot->project_count = model->project_count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->project_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_project_language_matrix_count(model->projects[index]) > 0U)
            out_snapshot->configured_project_count += 1U;
    }
    out_snapshot->revision = model->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer universal model languages operation used by this module and its
 * client applications.
 */
UmiLanguageProfileRegistry *umi_developer_universal_model_languages(
    UmiDeveloperUniversalModel *model)
{
    return model != NULL ? model->languages : NULL;
}

/*
 * Provide the developer universal model toolchains operation used by this module and its
 * client applications.
 */
UmiDeveloperToolchainBindingRegistry *
umi_developer_universal_model_toolchains(UmiDeveloperUniversalModel *model)
{
    return model != NULL ? model->toolchains : NULL;
}
