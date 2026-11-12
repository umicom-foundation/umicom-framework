/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/catalogue.c
 *
 * PURPOSE:
 *   Implement bounded multi-profile and per-project toolchain selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/catalogue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiToolchainCatalogue {
    UmiToolchainProfile profiles[UMI_TOOLCHAIN_CATALOGUE_CAPACITY];
    UmiToolchainProjectBindingSnapshot
        bindings[UMI_TOOLCHAIN_PROJECT_BINDING_CAPACITY];
    size_t profile_count;
    size_t binding_count;
    char selected_profile_id[UMI_TOOLCHAIN_TEXT_CAPACITY];
    uint64_t revision;
};

/*
 * Provide the find profile index operation used by this module and its client
 * applications.
 */
static bool ToolchainCatalogueIdentity(const char *text,size_t capacity)
{
    if (text == NULL || text[0] == '\0') return false;
    for (size_t index = 0U; index < capacity; ++index)
        if (text[index] == '\0') return true;
    return false;
}

/* Bound catalogue identity lookup before comparing caller or stored text. The previous implementation is retained for review. */
#if 0
static size_t find_profile_index(const UmiToolchainCatalogue *catalogue,
                                 const char *profile_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || profile_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->profile_count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->profiles[index].profile_id, profile_id) == 0)
            return index;
    return SIZE_MAX;
}
#endif
static size_t find_profile_index(const UmiToolchainCatalogue *catalogue,
                                 const char *profile_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || !ToolchainCatalogueIdentity(profile_id,UMI_TOOLCHAIN_TEXT_CAPACITY) ||
        catalogue->profile_count > UMI_TOOLCHAIN_CATALOGUE_CAPACITY) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->profile_count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (ToolchainCatalogueIdentity(catalogue->profiles[index].profile_id,UMI_TOOLCHAIN_TEXT_CAPACITY) &&
            strcmp(catalogue->profiles[index].profile_id, profile_id) == 0)
            return index;
    return SIZE_MAX;
}

/*
 * Provide the find binding index operation used by this module and its client
 * applications.
 */
/* Bound project identifiers before binding lookup. The previous implementation is retained for review. */
#if 0
static size_t find_binding_index(const UmiToolchainCatalogue *catalogue,
                                 const char *project_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || project_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->binding_count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->bindings[index].project_id, project_id) == 0)
            return index;
    return SIZE_MAX;
}
#endif
static size_t find_binding_index(const UmiToolchainCatalogue *catalogue,
                                 const char *project_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || !ToolchainCatalogueIdentity(project_id,128U) ||
        catalogue->binding_count > UMI_TOOLCHAIN_PROJECT_BINDING_CAPACITY) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->binding_count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (ToolchainCatalogueIdentity(catalogue->bindings[index].project_id,128U) &&
            strcmp(catalogue->bindings[index].project_id, project_id) == 0)
            return index;
    return SIZE_MAX;
}

/*
 * Provide the profile is candidate operation used by this module and its client
 * applications.
 */
static int profile_is_candidate(const UmiToolchainProfile *profile,
                                int require_c23)
{
    UmiToolchainCapabilitySnapshot capability;
    return profile != NULL &&
           umi_toolchain_capability_snapshot(profile, &capability) ==
               UMI_STATUS_OK &&
           umi_toolchain_capability_compatible(
               &capability, require_c23, 1, 1, 0);
}

/*
 * Initialise toolchain catalogue from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_toolchain_catalogue_create(UmiToolchainCatalogue **out_catalogue)
{
    UmiToolchainCatalogue *catalogue;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = NULL;
    catalogue = (UmiToolchainCatalogue *)calloc(1U, sizeof(*catalogue));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    catalogue->revision = 1U;
    *out_catalogue = catalogue;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by toolchain catalogue so the same storage can be reused
 * safely.
 */
void umi_toolchain_catalogue_destroy(UmiToolchainCatalogue *catalogue)
{
    free(catalogue);
}

/*
 * Provide the toolchain catalogue snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_catalogue_snapshot(
    const UmiToolchainCatalogue *catalogue,
    UmiToolchainCatalogueSnapshot *out_snapshot)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_TOOLCHAIN_CATALOGUE_API_VERSION;
    out_snapshot->revision = catalogue->revision;
    out_snapshot->profile_count = catalogue->profile_count;
    out_snapshot->project_binding_count = catalogue->binding_count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->profile_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (catalogue->profiles[index].complete)
            out_snapshot->complete_profile_count += 1U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (catalogue->profiles[index].c23_capable)
            out_snapshot->c23_profile_count += 1U;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (catalogue->selected_profile_id[0] != '\0') {
        (void)snprintf(out_snapshot->selected_profile_id,
                       sizeof(out_snapshot->selected_profile_id), "%s",
                       catalogue->selected_profile_id);
        out_snapshot->has_selected_profile = 1;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the toolchain catalogue upsert profile operation used by this module and its
 * client applications.
 */
/* Admit complete stored profiles and refuse exhausted revision mutation before publishing an upsert. The previous implementation is retained for review. */
#if 0
UmiStatus umi_toolchain_catalogue_upsert_profile(
    UmiToolchainCatalogue *catalogue,
    const UmiToolchainProfile *profile)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || profile == NULL || profile->profile_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_profile_index(catalogue, profile->profile_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (catalogue->profile_count >= UMI_TOOLCHAIN_CATALOGUE_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = catalogue->profile_count++;
    } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (memcmp(&catalogue->profiles[index], profile,
                      sizeof(*profile)) == 0) {
        return UMI_STATUS_OK;
    }
    catalogue->profiles[index] = *profile;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_toolchain_catalogue_upsert_profile(UmiToolchainCatalogue *catalogue,
                                               const UmiToolchainProfile *profile)
{
    if (catalogue == NULL || !umi_toolchain_profile_storage_valid(profile) ||
        profile->profile_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = find_profile_index(catalogue,profile->profile_id);
    if (index != SIZE_MAX && memcmp(&catalogue->profiles[index],profile,sizeof(*profile)) == 0)
        return UMI_STATUS_OK;
    if (catalogue->revision == UINT64_MAX ||
        (index == SIZE_MAX && catalogue->profile_count >= UMI_TOOLCHAIN_CATALOGUE_CAPACITY))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (index == SIZE_MAX) index = catalogue->profile_count;
    (void)memmove(&catalogue->profiles[index],profile,sizeof(*profile));
    if (index == catalogue->profile_count) catalogue->profile_count += 1U;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the toolchain catalogue find profile operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_catalogue_find_profile(
    const UmiToolchainCatalogue *catalogue, const char *profile_id,
    UmiToolchainProfile *out_profile)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || profile_id == NULL || out_profile == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_profile_index(catalogue, profile_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_profile = catalogue->profiles[index];
    return UMI_STATUS_OK;
}

/*
 * Find toolchain catalogue profile while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_toolchain_catalogue_profile_at(
    const UmiToolchainCatalogue *catalogue, size_t index,
    UmiToolchainProfile *out_profile)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || out_profile == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= catalogue->profile_count) return UMI_STATUS_NOT_FOUND;
    *out_profile = catalogue->profiles[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by toolchain catalogue profile without changing
 * their state.
 */
size_t umi_toolchain_catalogue_profile_count(
    const UmiToolchainCatalogue *catalogue)
{
    return catalogue != NULL ? catalogue->profile_count : 0U;
}

/*
 * Provide the toolchain catalogue select profile operation used by this module and its
 * client applications.
 */
/* Explicit selection keeps its no-op behavior and refuses exhausted revision mutation. The previous implementation is retained for review. */
#if 0
UmiStatus umi_toolchain_catalogue_select_profile(
    UmiToolchainCatalogue *catalogue, const char *profile_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || profile_id == NULL || profile_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_profile_index(catalogue, profile_id) == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(catalogue->selected_profile_id, profile_id) == 0)
        return UMI_STATUS_OK;
    (void)snprintf(catalogue->selected_profile_id,
                   sizeof(catalogue->selected_profile_id), "%s", profile_id);
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_toolchain_catalogue_select_profile(
    UmiToolchainCatalogue *catalogue, const char *profile_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || !ToolchainCatalogueIdentity(profile_id,UMI_TOOLCHAIN_TEXT_CAPACITY))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_profile_index(catalogue, profile_id) == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(catalogue->selected_profile_id, profile_id) == 0)
        return UMI_STATUS_OK;
    if (catalogue->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)snprintf(catalogue->selected_profile_id,
                   sizeof(catalogue->selected_profile_id), "%s", profile_id);
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the toolchain catalogue resolve selected profile operation used by this module
 * and its client applications.
 */
UmiStatus umi_toolchain_catalogue_resolve_selected_profile(
    const UmiToolchainCatalogue *catalogue, int require_c23,
    UmiToolchainProfile *out_profile)
{
    size_t index;
    const UmiToolchainProfile *best = NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || out_profile == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (catalogue->selected_profile_id[0] != '\0') {
        index = find_profile_index(catalogue, catalogue->selected_profile_id);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index != SIZE_MAX &&
            profile_is_candidate(&catalogue->profiles[index], require_c23)) {
            *out_profile = catalogue->profiles[index];
            return UMI_STATUS_OK;
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->profile_count; ++index) {
        const UmiToolchainProfile *candidate = &catalogue->profiles[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!profile_is_candidate(candidate, require_c23)) continue;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (best == NULL || strcmp(candidate->profile_id, best->profile_id) < 0)
            best = candidate;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (best == NULL) return UMI_STATUS_NOT_FOUND;
    *out_profile = *best;
    return UMI_STATUS_OK;
}

/*
 * Provide the toolchain catalogue bind project operation used by this module and its
 * client applications.
 */
/* Refuse malformed binding text and exhausted revisions before changing catalogue records; preserve normalized binding metadata. The previous implementation is retained for review. */
#if 0
UmiStatus umi_toolchain_catalogue_bind_project(
    UmiToolchainCatalogue *catalogue,
    const UmiToolchainProjectBindingSnapshot *binding)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || binding == NULL || binding->project_id[0] == '\0' ||
        binding->profile_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_profile_index(catalogue, binding->profile_id) == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    index = find_binding_index(catalogue, binding->project_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (catalogue->binding_count >= UMI_TOOLCHAIN_PROJECT_BINDING_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = catalogue->binding_count++;
    }
    catalogue->bindings[index] = *binding;
    catalogue->bindings[index].struct_size =
        (uint32_t)sizeof(catalogue->bindings[index]);
    catalogue->bindings[index].api_version = UMI_TOOLCHAIN_CATALOGUE_API_VERSION;
    catalogue->bindings[index].project_id[127U] = '\0';
    catalogue->bindings[index].profile_id[UMI_TOOLCHAIN_TEXT_CAPACITY - 1U] = '\0';
    catalogue->bindings[index].compilation_database[UMI_TOOL_PATH_CAPACITY - 1U] =
        '\0';
    catalogue->bindings[index].revision = ++catalogue->revision;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_toolchain_catalogue_bind_project(
    UmiToolchainCatalogue *catalogue,
    const UmiToolchainProjectBindingSnapshot *binding)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || binding == NULL ||
        !ToolchainCatalogueIdentity(binding->project_id,sizeof(binding->project_id)) ||
        !ToolchainCatalogueIdentity(binding->profile_id,sizeof(binding->profile_id)) ||
        !memchr(binding->compilation_database,' ',sizeof(binding->compilation_database)))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_profile_index(catalogue, binding->profile_id) == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    if (catalogue->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    index = find_binding_index(catalogue, binding->project_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (catalogue->binding_count >= UMI_TOOLCHAIN_PROJECT_BINDING_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        index = catalogue->binding_count++;
    }
    catalogue->bindings[index] = *binding;
    catalogue->bindings[index].struct_size =
        (uint32_t)sizeof(catalogue->bindings[index]);
    catalogue->bindings[index].api_version = UMI_TOOLCHAIN_CATALOGUE_API_VERSION;
    catalogue->bindings[index].project_id[127U] = '\0';
    catalogue->bindings[index].profile_id[UMI_TOOLCHAIN_TEXT_CAPACITY - 1U] = '\0';
    catalogue->bindings[index].compilation_database[UMI_TOOL_PATH_CAPACITY - 1U] =
        '\0';
    catalogue->bindings[index].revision = ++catalogue->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the toolchain catalogue resolve project profile operation used by this module
 * and its client applications.
 */
UmiStatus umi_toolchain_catalogue_resolve_project_profile(
    const UmiToolchainCatalogue *catalogue, const char *project_id,
    int require_c23, UmiToolchainProfile *out_profile)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || project_id == NULL || out_profile == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_binding_index(catalogue, project_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index != SIZE_MAX) {
        UmiStatus status = umi_toolchain_catalogue_find_profile(
            catalogue, catalogue->bindings[index].profile_id, out_profile);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        return profile_is_candidate(out_profile, require_c23)
            ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
    }
    return umi_toolchain_catalogue_resolve_selected_profile(
        catalogue, require_c23, out_profile);
}

/*
 * Find toolchain catalogue project binding while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_toolchain_catalogue_project_binding_at(
    const UmiToolchainCatalogue *catalogue, size_t index,
    UmiToolchainProjectBindingSnapshot *out_binding)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || out_binding == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= catalogue->binding_count) return UMI_STATUS_NOT_FOUND;
    *out_binding = catalogue->bindings[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the toolchain catalogue discover operation used by this module and its client
 * applications.
 */
/* Refuse a missing explicit-root array before discovery traverses it. The previous implementation is retained for review. */
#if 0
UmiStatus umi_toolchain_catalogue_discover(
    UmiToolchainCatalogue *catalogue,
    const UmiToolchainCatalogueDiscoveryRequest *request,
    UmiToolchainCatalogueDiscoverySnapshot *out_snapshot)
{
    static const char *const default_profiles[] = {
#ifdef _WIN32
        "windows-ucrt64-clang", "windows-ucrt64-gcc",
        "windows-clang64-clang", "windows-mingw64-gcc", "windows-msvc"
#else
        "posix-clang", "posix-gcc"
#endif
    };
    size_t root_count;
    size_t root_index;
    size_t profile_index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_TOOLCHAIN_CATALOGUE_API_VERSION;
    root_count = request != NULL && request->explicit_root_count != 0U
        ? request->explicit_root_count : 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (root_count > UMI_TOOLCHAIN_DISCOVERY_ROOT_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Visit each bounded item once so every record receives the same rule. */
    for (root_index = 0U; root_index < root_count; ++root_index) {
        const char *root = request != NULL && request->explicit_root_count != 0U
            ? request->explicit_roots[root_index] : NULL;
        /* Visit each bounded item once so every record receives the same rule. */
        for (profile_index = 0U;
             profile_index < sizeof(default_profiles) / sizeof(default_profiles[0]);
             ++profile_index) {
            UmiToolchainDiscoveryRequest item;
            UmiToolchainDiscoveryReport report;
            UmiStatus status;
            (void)memset(&item, 0, sizeof(item));
            item.explicit_root = root;
            item.preferred_profile = default_profiles[profile_index];
            item.require_gtk = request != NULL ? request->require_gtk : 0;
            item.require_java = request != NULL ? request->require_java : 0;
            item.require_github_cli =
                request != NULL ? request->require_github_cli : 0;
            item.skip_compile_probe =
                request == NULL || !request->run_compile_probe;
            item.diagnostic_sink = request != NULL ? request->diagnostic_sink : NULL;
            item.diagnostic_user_data =
                request != NULL ? request->diagnostic_user_data : NULL;
            out_snapshot->attempted_profile_count += 1U;
            status = umi_toolchain_discover(&item, &report);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK && !report.profile.complete) {
                out_snapshot->rejected_profile_count += 1U;
                continue;
            }
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (status != UMI_STATUS_OK && request != NULL && request->require_c23) {
                out_snapshot->rejected_profile_count += 1U;
                continue;
            }
            status = umi_toolchain_catalogue_upsert_profile(
                catalogue, &report.profile);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
            out_snapshot->discovered_profile_count += 1U;
            /* Apply this branch only when its contract condition is satisfied. */
            if (report.profile.complete) out_snapshot->complete_profile_count += 1U;
            /* Apply this branch only when its contract condition is satisfied. */
            if (report.profile.c23_capable) out_snapshot->c23_profile_count += 1U;
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request != NULL && request->preferred_profile != NULL &&
        find_profile_index(catalogue, request->preferred_profile) != SIZE_MAX)
        (void)umi_toolchain_catalogue_select_profile(
            catalogue, request->preferred_profile);
    {
        UmiToolchainProfile selected;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_toolchain_catalogue_resolve_selected_profile(
                catalogue, request != NULL ? request->require_c23 : 0,
                &selected) == UMI_STATUS_OK) {
            (void)snprintf(out_snapshot->selected_profile_id,
                           sizeof(out_snapshot->selected_profile_id), "%s",
                           selected.profile_id);
        }
    }
    return out_snapshot->discovered_profile_count != 0U
        ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
#endif
UmiStatus umi_toolchain_catalogue_discover(
    UmiToolchainCatalogue *catalogue,
    const UmiToolchainCatalogueDiscoveryRequest *request,
    UmiToolchainCatalogueDiscoverySnapshot *out_snapshot)
{
    static const char *const default_profiles[] = {
#ifdef _WIN32
        "windows-ucrt64-clang", "windows-ucrt64-gcc",
        "windows-clang64-clang", "windows-mingw64-gcc", "windows-msvc"
#else
        "posix-clang", "posix-gcc"
#endif
    };
    size_t root_count;
    size_t root_index;
    size_t profile_index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (request != NULL && request->explicit_root_count != 0U && request->explicit_roots == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_TOOLCHAIN_CATALOGUE_API_VERSION;
    root_count = request != NULL && request->explicit_root_count != 0U
        ? request->explicit_root_count : 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (root_count > UMI_TOOLCHAIN_DISCOVERY_ROOT_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Visit each bounded item once so every record receives the same rule. */
    for (root_index = 0U; root_index < root_count; ++root_index) {
        const char *root = request != NULL && request->explicit_root_count != 0U
            ? request->explicit_roots[root_index] : NULL;
        /* Visit each bounded item once so every record receives the same rule. */
        for (profile_index = 0U;
             profile_index < sizeof(default_profiles) / sizeof(default_profiles[0]);
             ++profile_index) {
            UmiToolchainDiscoveryRequest item;
            UmiToolchainDiscoveryReport report;
            UmiStatus status;
            (void)memset(&item, 0, sizeof(item));
            item.explicit_root = root;
            item.preferred_profile = default_profiles[profile_index];
            item.require_gtk = request != NULL ? request->require_gtk : 0;
            item.require_java = request != NULL ? request->require_java : 0;
            item.require_github_cli =
                request != NULL ? request->require_github_cli : 0;
            item.skip_compile_probe =
                request == NULL || !request->run_compile_probe;
            item.diagnostic_sink = request != NULL ? request->diagnostic_sink : NULL;
            item.diagnostic_user_data =
                request != NULL ? request->diagnostic_user_data : NULL;
            out_snapshot->attempted_profile_count += 1U;
            status = umi_toolchain_discover(&item, &report);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK && !report.profile.complete) {
                out_snapshot->rejected_profile_count += 1U;
                continue;
            }
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (status != UMI_STATUS_OK && request != NULL && request->require_c23) {
                out_snapshot->rejected_profile_count += 1U;
                continue;
            }
            status = umi_toolchain_catalogue_upsert_profile(
                catalogue, &report.profile);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
            out_snapshot->discovered_profile_count += 1U;
            /* Apply this branch only when its contract condition is satisfied. */
            if (report.profile.complete) out_snapshot->complete_profile_count += 1U;
            /* Apply this branch only when its contract condition is satisfied. */
            if (report.profile.c23_capable) out_snapshot->c23_profile_count += 1U;
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request != NULL && request->preferred_profile != NULL &&
        find_profile_index(catalogue, request->preferred_profile) != SIZE_MAX)
        (void)umi_toolchain_catalogue_select_profile(
            catalogue, request->preferred_profile);
    {
        UmiToolchainProfile selected;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_toolchain_catalogue_resolve_selected_profile(
                catalogue, request != NULL ? request->require_c23 : 0,
                &selected) == UMI_STATUS_OK) {
            (void)snprintf(out_snapshot->selected_profile_id,
                           sizeof(out_snapshot->selected_profile_id), "%s",
                           selected.profile_id);
        }
    }
    return out_snapshot->discovered_profile_count != 0U
        ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}


static bool ToolchainSelectionOverlap(const void *first,size_t first_size,
                                      const void *second,size_t second_size)
{
    uintptr_t a = (uintptr_t)first, b = (uintptr_t)second;
    return b >= a ? b-a < first_size : a-b < second_size;
}
typedef struct UmiToolchainPreparedSelection {
    UmiToolchainProfile profile;
    UmiEnvironmentPlan environment;
} UmiToolchainPreparedSelection;

UmiStatus umi_toolchain_catalogue_select_with_environment(UmiToolchainCatalogue *catalogue,
    const char *profile_id,UmiToolchainProfile *out_profile,UmiEnvironmentPlan *out_environment)
{
    if (catalogue == NULL || out_profile == NULL || out_environment == NULL ||
        !ToolchainCatalogueIdentity(profile_id,UMI_TOOLCHAIN_TEXT_CAPACITY) ||
        ToolchainSelectionOverlap(catalogue,sizeof(*catalogue),out_profile,sizeof(*out_profile)) ||
        ToolchainSelectionOverlap(catalogue,sizeof(*catalogue),out_environment,sizeof(*out_environment)) ||
        ToolchainSelectionOverlap(out_profile,sizeof(*out_profile),out_environment,sizeof(*out_environment)))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiToolchainPreparedSelection *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = umi_toolchain_catalogue_find_profile(catalogue,profile_id,&candidate->profile);
    if (status == UMI_STATUS_OK)
        status = umi_environment_plan_from_toolchain(&candidate->profile,&candidate->environment);
    /* Selection changes only after all environment allocation/admission succeeds.
     * Caller serialization keeps the selected row stable between these steps. */
    if (status == UMI_STATUS_OK)
        status = umi_toolchain_catalogue_select_profile(catalogue,candidate->profile.profile_id);
    if (status == UMI_STATUS_OK) {
        *out_profile = candidate->profile;
        *out_environment = candidate->environment;
        (void)umi_environment_plan_variables(out_environment);
    }
    free(candidate);
    return status;
}
