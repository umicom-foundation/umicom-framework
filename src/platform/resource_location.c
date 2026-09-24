/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/resource_location.c
 *
 * PURPOSE:
 *   Implement resource-location snapshots and resolve writable per-user
 *   application directories without using an application's installation
 *   folder or the process working directory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Each operation is deliberately small and deterministic. Snapshots are copied
 * into bounded storage, revisions advance on mutation, and callers retain
 * responsibility for higher-level threading and persistence policy.
 */
#include "umicom/platform/resource_location.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/platform/filesystem.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

struct UmiResourceLocationRegistry {
    UmiResourceLocationSnapshot items[UMI_PLATFORM_RESOURCE_LOCATION_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiResourceLocationRegistry *registry, const char *id)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

/*
 * Initialise platform resource location registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_platform_resource_location_registry_create(UmiResourceLocationRegistry **out_registry)
{
    UmiResourceLocationRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiResourceLocationRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by platform resource location registry so the same storage
 * can be reused safely.
 */
void umi_platform_resource_location_registry_destroy(UmiResourceLocationRegistry *registry)
{
    free(registry);
}

/*
 * Provide the platform resource location registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_platform_resource_location_registry_upsert(UmiResourceLocationRegistry *registry, const UmiResourceLocationSnapshot *item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->count >= UMI_PLATFORM_RESOURCE_LOCATION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiResourceLocationSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

/*
 * Remove platform resource location registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_platform_resource_location_registry_remove(UmiResourceLocationRegistry *registry, const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < registry->count) {
        memmove(&registry->items[index], &registry->items[index + 1U],
                (registry->count - index - 1U) * sizeof(registry->items[0]));
    }
    registry->count -= 1U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find platform resource location registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_platform_resource_location_registry_find(const UmiResourceLocationRegistry *registry, const char *id, UmiResourceLocationSnapshot *out_item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find platform resource location registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_platform_resource_location_registry_at(const UmiResourceLocationRegistry *registry, size_t index, UmiResourceLocationSnapshot *out_item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by platform resource location registry without
 * changing their state.
 */
size_t umi_platform_resource_location_registry_count(const UmiResourceLocationRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the platform resource location registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_platform_resource_location_registry_revision(const UmiResourceLocationRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

/*
 * Writable application directories are resolved here because resource and path
 * discovery are reusable Framework responsibilities.  The resource-location
 * registry above remains unchanged; this extension is additive so existing
 * callers keep the same behaviour and identifiers.
 */
/* Application directory names are single path components, never paths. */
static int ApplicationDirectoryNameValid(const char *value)
{
    size_t index;
    size_t length;

    if (value == NULL || value[0] == '\0') return 0;
    if (strcmp(value, ".") == 0 || strcmp(value, "..") == 0) return 0;
    length = strlen(value);
    if (length == 0U || length >= 128U) return 0;
    if (value[length - 1U] == ' ' || value[length - 1U] == '.') return 0;

    for (index = 0U; index < length; ++index) {
        const unsigned char character = (unsigned char)value[index];
        if (character < 32U ||
            character == '<' || character == '>' || character == ':' ||
            character == '"' || character == '/' || character == '\\' ||
            character == '|' || character == '?' || character == '*') {
            return 0;
        }
    }
    return 1;
}

/* Join base/organisation/application while preserving the Framework path rules. */
static UmiStatus ApplicationDirectoryPath(
    const char *base,
    const char *organisationDirectory,
    const char *applicationDirectory,
    char *outPath,
    size_t capacity)
{
    char organisationPath[UMI_PATH_CAPACITY];
    UmiStatus status;

    status = umi_path_join(base,
                           organisationDirectory,
                           organisationPath,
                           sizeof(organisationPath));
    if (status != UMI_STATUS_OK) return status;
    return umi_path_join(organisationPath,
                         applicationDirectory,
                         outPath,
                         capacity);
}

/* Resolve one named child below an application-owned directory. */
static UmiStatus ApplicationChildPath(
    const char *root,
    const char *child,
    char *outPath)
{
    return umi_path_join(root, child, outPath, UMI_PATH_CAPACITY);
}

#ifdef _WIN32
/* Read a Windows path environment value through the Unicode Win32 API. */
static UmiStatus WindowsEnvironmentPath(
    const wchar_t *name,
    char *outPath,
    size_t capacity)
{
    wchar_t *nativeValue = NULL;
    DWORD nativeCount;
    int utf8Count;
    UmiStatus status = UMI_STATUS_OK;

    if (name == NULL || outPath == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    outPath[0] = '\0';
    nativeCount = GetEnvironmentVariableW(name, NULL, 0U);
    if (nativeCount == 0U) return UMI_STATUS_NOT_FOUND;

    nativeValue = (wchar_t *)calloc((size_t)nativeCount, sizeof(*nativeValue));
    if (nativeValue == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    {
        DWORD copiedCount = GetEnvironmentVariableW(
            name, nativeValue, nativeCount);
        if (copiedCount == 0U) {
            status = UMI_STATUS_IO_ERROR;
            goto done;
        }
        /* The environment can change between the sizing call and this read.
         * Reject a newly enlarged value rather than converting an incomplete
         * native path. A later call can resolve the new value safely. */
        if (copiedCount >= nativeCount) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            goto done;
        }
    }

    utf8Count = WideCharToMultiByte(CP_UTF8,
                                    WC_ERR_INVALID_CHARS,
                                    nativeValue,
                                    -1,
                                    NULL,
                                    0,
                                    NULL,
                                    NULL);
    if (utf8Count <= 0) {
        status = UMI_STATUS_IO_ERROR;
        goto done;
    }
    if ((size_t)utf8Count > capacity) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
    }
    if (WideCharToMultiByte(CP_UTF8,
                            WC_ERR_INVALID_CHARS,
                            nativeValue,
                            -1,
                            outPath,
                            utf8Count,
                            NULL,
                            NULL) != utf8Count) {
        status = UMI_STATUS_IO_ERROR;
        goto done;
    }
    status = umi_path_normalise(outPath, outPath, capacity);

done:
    free(nativeValue);
    if (status != UMI_STATUS_OK) outPath[0] = '\0';
    return status;
}
#else
/* Resolve an XDG directory or its conventional location below HOME. */
static UmiStatus PosixUserBase(
    const char *environmentName,
    const char *homeRelative,
    char *outPath,
    size_t capacity)
{
    const char *value;
    const char *home;

    if (environmentName == NULL || homeRelative == NULL ||
        outPath == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    value = getenv(environmentName);
    if (value != NULL && value[0] != '\0') {
        if (!umi_path_is_absolute(value)) return UMI_STATUS_INVALID_ARGUMENT;
        return umi_path_normalise(value, outPath, capacity);
    }

    home = getenv("HOME");
    if (home == NULL || home[0] == '\0' || !umi_path_is_absolute(home)) {
        return UMI_STATUS_NOT_FOUND;
    }
    return umi_path_join(home, homeRelative, outPath, capacity);
}
#endif

UmiApplicationPathsConfig UmiApplicationPathsConfigDefault(
    const char *applicationDirectory)
{
    UmiApplicationPathsConfig config;

    config.structSize = (uint32_t)sizeof(config);
    config.apiVersion = UMI_APPLICATION_PATHS_API_VERSION;
    config.organisationDirectory = "Umicom";
    config.applicationDirectory = applicationDirectory;
    config.baseOverride = NULL;
    return config;
}

/* Build config/state/cache/data paths from one explicit application root. */
static UmiStatus ResolveSingleRootApplicationPaths(
    const char *base,
    const UmiApplicationPathsConfig *config,
    UmiApplicationPaths *outPaths)
{
    UmiStatus status;

    status = ApplicationDirectoryPath(base,
                                      config->organisationDirectory,
                                      config->applicationDirectory,
                                      outPaths->root,
                                      sizeof(outPaths->root));
    if (status != UMI_STATUS_OK) return status;
    status = ApplicationChildPath(outPaths->root, "config", outPaths->config);
    if (status != UMI_STATUS_OK) return status;
    status = ApplicationChildPath(outPaths->root, "state", outPaths->state);
    if (status != UMI_STATUS_OK) return status;
    status = ApplicationChildPath(outPaths->root, "cache", outPaths->cache);
    if (status != UMI_STATUS_OK) return status;
    status = ApplicationChildPath(outPaths->root, "data", outPaths->data);
    if (status != UMI_STATUS_OK) return status;
    status = ApplicationChildPath(outPaths->root, "logs", outPaths->logs);
    if (status != UMI_STATUS_OK) return status;
    status = ApplicationChildPath(outPaths->state,
                                  "recovery",
                                  outPaths->recovery);
    return status;
}

UmiStatus UmiApplicationPathsResolve(
    const UmiApplicationPathsConfig *config,
    UmiApplicationPaths *outPaths)
{
    UmiStatus status;

    if (config == NULL || outPaths == NULL ||
        config->structSize != (uint32_t)sizeof(*config) ||
        config->apiVersion != UMI_APPLICATION_PATHS_API_VERSION ||
        !ApplicationDirectoryNameValid(config->organisationDirectory) ||
        !ApplicationDirectoryNameValid(config->applicationDirectory)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(outPaths, 0, sizeof(*outPaths));
    outPaths->structSize = (uint32_t)sizeof(*outPaths);
    outPaths->apiVersion = UMI_APPLICATION_PATHS_API_VERSION;

    if (config->baseOverride != NULL && config->baseOverride[0] != '\0') {
        char overridePath[UMI_PATH_CAPACITY];
        if (!umi_path_is_absolute(config->baseOverride)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        status = umi_path_normalise(config->baseOverride,
                                    overridePath,
                                    sizeof(overridePath));
        if (status != UMI_STATUS_OK) return status;
        return ResolveSingleRootApplicationPaths(overridePath,
                                                 config,
                                                 outPaths);
    }

#ifdef _WIN32
    {
        char localAppData[UMI_PATH_CAPACITY];
        status = WindowsEnvironmentPath(L"LOCALAPPDATA",
                                        localAppData,
                                        sizeof(localAppData));
        if (status != UMI_STATUS_OK) return status;
        if (!umi_path_is_absolute(localAppData)) {
            return UMI_STATUS_INVALID_STATE;
        }
        return ResolveSingleRootApplicationPaths(localAppData,
                                                 config,
                                                 outPaths);
    }
#else
    {
        char configBase[UMI_PATH_CAPACITY];
        char dataBase[UMI_PATH_CAPACITY];
        char stateBase[UMI_PATH_CAPACITY];
        char cacheBase[UMI_PATH_CAPACITY];

        status = PosixUserBase("XDG_CONFIG_HOME", ".config",
                               configBase, sizeof(configBase));
        if (status != UMI_STATUS_OK) return status;
        status = PosixUserBase("XDG_DATA_HOME", ".local/share",
                               dataBase, sizeof(dataBase));
        if (status != UMI_STATUS_OK) return status;
        status = PosixUserBase("XDG_STATE_HOME", ".local/state",
                               stateBase, sizeof(stateBase));
        if (status != UMI_STATUS_OK) return status;
        status = PosixUserBase("XDG_CACHE_HOME", ".cache",
                               cacheBase, sizeof(cacheBase));
        if (status != UMI_STATUS_OK) return status;

        status = ApplicationDirectoryPath(
            stateBase,
            config->organisationDirectory,
            config->applicationDirectory,
            outPaths->root,
            sizeof(outPaths->root));
        if (status != UMI_STATUS_OK) return status;
        status = ApplicationDirectoryPath(
            configBase,
            config->organisationDirectory,
            config->applicationDirectory,
            outPaths->config,
            sizeof(outPaths->config));
        if (status != UMI_STATUS_OK) return status;
        status = ApplicationDirectoryPath(
            stateBase,
            config->organisationDirectory,
            config->applicationDirectory,
            outPaths->state,
            sizeof(outPaths->state));
        if (status != UMI_STATUS_OK) return status;
        status = ApplicationDirectoryPath(
            cacheBase,
            config->organisationDirectory,
            config->applicationDirectory,
            outPaths->cache,
            sizeof(outPaths->cache));
        if (status != UMI_STATUS_OK) return status;
        status = ApplicationDirectoryPath(
            dataBase,
            config->organisationDirectory,
            config->applicationDirectory,
            outPaths->data,
            sizeof(outPaths->data));
        if (status != UMI_STATUS_OK) return status;
        status = ApplicationChildPath(outPaths->state,
                                      "logs",
                                      outPaths->logs);
        if (status != UMI_STATUS_OK) return status;
        status = ApplicationChildPath(outPaths->state,
                                      "recovery",
                                      outPaths->recovery);
        return status;
    }
#endif
}

UmiStatus UmiApplicationPathsPrepare(const UmiApplicationPaths *paths)
{
    const char *directories[] = {
        paths != NULL ? paths->root : NULL,
        paths != NULL ? paths->config : NULL,
        paths != NULL ? paths->state : NULL,
        paths != NULL ? paths->cache : NULL,
        paths != NULL ? paths->data : NULL,
        paths != NULL ? paths->logs : NULL,
        paths != NULL ? paths->recovery : NULL
    };
    size_t index;

    if (paths == NULL ||
        paths->structSize != (uint32_t)sizeof(*paths) ||
        paths->apiVersion != UMI_APPLICATION_PATHS_API_VERSION ||
        paths->root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < sizeof(directories) / sizeof(directories[0]); ++index) {
        UmiStatus status;
        if (directories[index] == NULL || directories[index][0] == '\0') {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        status = umi_fs_make_directories(directories[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
