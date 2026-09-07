/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime_catalogue_native_discovery.inc
 * PURPOSE: Transactional native presence reconciliation in the existing owner.
 * AUTHOR: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <errno.h>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include "umicom/platform/directory.h"
#include <unistd.h>
#endif

/* Supply inert, portable defaults; enabling a trusted location is explicit. */
UmiApplicationNativeDiscoveryConfig umi_application_native_discovery_config_default(void)
{
    UmiApplicationNativeDiscoveryConfig config = {0};
    config.structure_size = sizeof(config);
#ifdef _WIN32
    config.executable_suffix = ".exe";
#else
    config.executable_suffix = "";
#endif
    config.interval_ms = 1000U;
    return config;
}

/* Validate bounded absolute paths without resolving or searching any directory. */
UmiStatus umi_application_native_discovery_config_validate(
    const UmiApplicationNativeDiscoveryConfig *config)
{
    const char *root;
    const char *segment;
    const char *cursor;
    size_t length;
    if (config == NULL || config->structure_size < sizeof(*config) ||
        config->executable_root == NULL || config->executable_suffix == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    root = config->executable_root;
    length = strlen(root);
    if (length == 0U || length >= UMI_APPLICATION_RUNTIME_PATH_CAPACITY - 256U)
        return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    if (!(length >= 3U && ((root[0] >= 'A' && root[0] <= 'Z') ||
           (root[0] >= 'a' && root[0] <= 'z')) && root[1] == ':' &&
           (root[2] == '/' || root[2] == '\\')))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(config->executable_suffix, ".exe") != 0)
        return UMI_STATUS_INVALID_ARGUMENT;
#else
    if (root[0] != '/' || root[1] == '/' || strcmp(config->executable_suffix, "") != 0)
        return UMI_STATUS_INVALID_ARGUMENT;
#endif
    segment = root;
    for (cursor = root; ; ++cursor) {
        if (*cursor != '\0' && (unsigned char)*cursor < 32U)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (*cursor == '/' || *cursor == '\\' || *cursor == '\0') {
            const size_t count = (size_t)(cursor - segment);
            if ((count == 1U && segment[0] == '.') ||
                (count == 2U && segment[0] == '.' && segment[1] == '.'))
                return UMI_STATUS_INVALID_ARGUMENT;
            if (*cursor == '\0') break;
            segment = cursor + 1;
        }
    }
    return UMI_STATUS_OK;
}

/* The platform stat boundary distinguishes missing files from scan failures. */
static UmiStatus native_file_probe(const char *path, bool *out_present, void *context)
{
#ifdef _WIN32
    wchar_t *wide;
    DWORD attributes;
    DWORD error;
    int count;
    (void)context;
    *out_present = false;
    count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, NULL, 0);
    if (count <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    wide = (wchar_t *)malloc((size_t)count * sizeof(*wide));
    if (wide == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide, count) != count) {
        free(wide);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    attributes = GetFileAttributesW(wide);
    error = attributes == INVALID_FILE_ATTRIBUTES ? GetLastError() : ERROR_SUCCESS;
    free(wide);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) return UMI_STATUS_OK;
        return error == ERROR_ACCESS_DENIED ? UMI_STATUS_PERMISSION_DENIED : UMI_STATUS_IO_ERROR;
    }
    *out_present = (attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) == 0U;
    return UMI_STATUS_OK;
#else
    UmiFileInfo *info;
    UmiStatus status;
    int saved_errno;
    (void)context;
    *out_present = false;
    info = (UmiFileInfo *)malloc(sizeof(*info));
    if (info == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_directory_stat(path, info);
    saved_errno = errno;
    if (status == UMI_STATUS_OK) *out_present = info->kind == UMI_FILE_KIND_REGULAR;
    free(info);
    if (status == UMI_STATUS_NOT_FOUND) {
        if (saved_errno == ENOENT || saved_errno == ENOTDIR) return UMI_STATUS_OK;
        return saved_errno == EACCES ? UMI_STATUS_PERMISSION_DENIED : UMI_STATUS_IO_ERROR;
    }
    if (status == UMI_STATUS_OK && *out_present && access(path, X_OK) != 0) {
        if (errno == EACCES || errno == ENOENT || errno == ENOTDIR) *out_present = false;
        else return UMI_STATUS_IO_ERROR;
    }
    return status;
#endif
}

/* Resolve only the established built-in GUI companion under the trusted root. */
UmiStatus umi_application_native_discovery_probe(
    const UmiApplicationNativeDiscoveryConfig *config,
    const char *application_id, bool *out_present)
{
    const char *name;
    char path[UMI_APPLICATION_RUNTIME_PATH_CAPACITY];
    int count;
    UmiStatus status;
    if (out_present == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_present = false;
    status = umi_application_native_discovery_config_validate(config);
    if (status != UMI_STATUS_OK) return status;
    name = umi_application_portfolio_gui_executable(application_id);
    if (name == NULL) return UMI_STATUS_NOT_FOUND;
    count = snprintf(path, sizeof(path), "%s/%s%s", config->executable_root,
                     name, config->executable_suffix);
    if (count < 0 || (size_t)count >= sizeof(path)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return (config->probe != NULL ? config->probe : native_file_probe)(
        path, out_present, config->probe_context);
}

/* Prepare explicit initial policy without adding a second product registry. */
UmiStatus umi_application_runtime_catalogue_admit_native_portfolio(
    UmiApplicationRuntimeCatalogue *catalogue)
{
    UmiApplicationRuntimeCatalogue *candidate;
    size_t index;
    size_t changed = 0U;
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    candidate = (UmiApplicationRuntimeCatalogue *)malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *catalogue;
    for (index = 0U; index < candidate->count; ++index) {
        UmiApplicationRuntimeRecord *record = &candidate->records[index];
        const char *name = umi_application_portfolio_gui_executable(record->application_id);
        if (name == NULL) continue;
        if (strcmp(record->executable_name, name) == 0 && record->compatible && record->enabled)
            continue;
        if (record->revision == UINT64_MAX || candidate->revision == UINT64_MAX) {
            free(candidate);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)copy_text(record->executable_name, sizeof(record->executable_name), name, false);
        record->compatible = true;
        record->enabled = true;
        update_visibility(record);
        if (!record->active && strcmp(candidate->active_application_id, record->application_id) == 0)
            candidate->active_application_id[0] = '\0';
        ++record->revision;
        ++changed;
    }
    if (changed != 0U) {
        ++candidate->revision;
        *catalogue = *candidate;
    }
    free(candidate);
    return UMI_STATUS_OK;
}

/* Every observation is staged; missing binaries never erase living processes. */
UmiStatus umi_application_runtime_catalogue_discover_native(
    UmiApplicationRuntimeCatalogue *catalogue,
    const UmiApplicationNativeDiscoveryConfig *config,
    UmiApplicationNativeDiscoveryReport *out_report)
{
    UmiApplicationRuntimeCatalogue *candidate;
    UmiApplicationNativeDiscoveryReport report = {0};
    UmiStatus status;
    size_t index;
    if (catalogue == NULL || out_report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    report.revision = catalogue->revision;
    status = umi_application_native_discovery_config_validate(config);
    if (status != UMI_STATUS_OK) {
        report.status = status;
        *out_report = report;
        return status;
    }
    candidate = (UmiApplicationRuntimeCatalogue *)malloc(sizeof(*candidate));
    if (candidate == NULL) {
        report.status = UMI_STATUS_OUT_OF_MEMORY;
        *out_report = report;
        return report.status;
    }
    *candidate = *catalogue;
    for (index = 0U; index < candidate->count; ++index) {
        UmiApplicationRuntimeRecord *record = &candidate->records[index];
        const char *name = umi_application_portfolio_gui_executable(record->application_id);
        bool present = false;
        if (name == NULL) continue;
        ++report.scanned_count;
        status = umi_application_native_discovery_probe(config, record->application_id, &present);
        if (status != UMI_STATUS_OK) break;
        if (present) ++report.installed_count;
        if (record->installed == present && strcmp(record->executable_name, name) == 0) continue;
        if (record->revision == UINT64_MAX || candidate->revision == UINT64_MAX) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        record->installed = present;
        (void)copy_text(record->executable_name, sizeof(record->executable_name), name, false);
        update_visibility(record);
        if (!record->active && strcmp(candidate->active_application_id, record->application_id) == 0)
            candidate->active_application_id[0] = '\0';
        ++record->revision;
        ++report.changed_count;
    }
    if (status == UMI_STATUS_OK && report.changed_count != 0U) {
        ++candidate->revision;
        *catalogue = *candidate;
        report.revision = catalogue->revision;
    } else if (status != UMI_STATUS_OK) {
        report.changed_count = 0U;
    }
    report.status = status;
    *out_report = report;
    free(candidate);
    return status;
}
