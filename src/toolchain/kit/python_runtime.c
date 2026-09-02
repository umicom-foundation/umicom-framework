/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/kit/python_runtime.c
 * PURPOSE: Implement deterministic CPython discovery and validation.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/python_runtime.h"

#include <stdio.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/process.h"
#include "umicom/toolchain/discovery.h"

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the first line operation used by this module and its client applications. */
static void first_line(char *text)
{
    char *end;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return;
    end = strpbrk(text, "\r\n");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (end != NULL) *end = '\0';
}

/* Provide the parent directory operation used by this module and its client applications. */
static void parent_directory(const char *path, char *out_parent, size_t capacity)
{
    char *slash;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (copy_text(out_parent, capacity, path) != UMI_STATUS_OK) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > 0U) out_parent[0] = '\0';
        return;
    }
    slash = strrchr(out_parent, '/');
#ifdef _WIN32
    {
        char *backslash = strrchr(out_parent, '\\');
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (backslash != NULL && (slash == NULL || backslash > slash)) slash = backslash;
    }
#endif
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (slash != NULL) *slash = '\0';
}

/* Provide the resolve in root operation used by this module and its client applications. */
static UmiStatus resolve_in_root(const char *root,
                                 const char *name,
                                 char *out_path,
                                 size_t capacity)
{
    char bin[UMI_TOOL_PATH_CAPACITY];
    char candidate[UMI_TOOL_PATH_CAPACITY];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || root[0] == '\0') return UMI_STATUS_NOT_FOUND;
    status = umi_fs_join(candidate, sizeof(candidate), root, name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && umi_fs_is_file(candidate))
        return copy_text(out_path, capacity, candidate);
    status = umi_fs_join(bin, sizeof(bin), root, "bin");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_fs_join(candidate, sizeof(candidate), bin, name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && umi_fs_is_file(candidate))
        return copy_text(out_path, capacity, candidate);
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Initialise python runtime request from caller-provided values so later operations
 * receive a known state.
 */
void umi_python_runtime_request_init(UmiPythonRuntimeRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL) return;
    (void)memset(request, 0, sizeof(*request));
    request->struct_size = (uint32_t)sizeof(*request);
    request->api_version = UMI_PYTHON_RUNTIME_API_VERSION;
    request->validate_version = 1;
}

/*
 * Provide the python runtime discover operation used by this module and its client
 * applications.
 */
UmiStatus umi_python_runtime_discover(const UmiPythonRuntimeRequest *request,
                                      UmiPythonRuntimeSnapshot *out_snapshot)
{
#ifdef _WIN32
    static const char *const defaults[] = {"python.exe", "python3.exe", "py.exe"};
#else
    static const char *const defaults[] = {"python3", "python"};
#endif
    const char *preferred;
    size_t index;
    UmiStatus status = UMI_STATUS_NOT_FOUND;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || out_snapshot == NULL ||
        request->struct_size < sizeof(*request) ||
        request->api_version != UMI_PYTHON_RUNTIME_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_PYTHON_RUNTIME_API_VERSION;

    preferred = request->preferred_executable;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preferred != NULL && preferred[0] != '\0') {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (strchr(preferred, '/') != NULL || strchr(preferred, '\\') != NULL) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (umi_fs_is_file(preferred))
                status = copy_text(out_snapshot->executable,
                                   sizeof(out_snapshot->executable), preferred);
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            status = resolve_in_root(request->explicit_root, preferred,
                                     out_snapshot->executable,
                                     sizeof(out_snapshot->executable));
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK)
                status = umi_toolchain_find_on_path(preferred,
                                                    out_snapshot->executable,
                                                    sizeof(out_snapshot->executable));
        }
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         status != UMI_STATUS_OK && index < sizeof(defaults) / sizeof(defaults[0]);
         ++index) {
        status = resolve_in_root(request->explicit_root, defaults[index],
                                 out_snapshot->executable,
                                 sizeof(out_snapshot->executable));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK)
            status = umi_toolchain_find_on_path(defaults[index],
                                                out_snapshot->executable,
                                                sizeof(out_snapshot->executable));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return UMI_STATUS_NOT_FOUND;

    out_snapshot->available = 1;
    parent_directory(out_snapshot->executable,
                     out_snapshot->home, sizeof(out_snapshot->home));
    /* Apply this operation only while the related capability or state is available. */
    if (request->validate_version != 0) {
        const char *arguments[] = {"--version"};
        int exit_code = -1;
        status = umi_process_capture(out_snapshot->executable,
                                     arguments, 1U,
                                     out_snapshot->version,
                                     sizeof(out_snapshot->version),
                                     &exit_code);
        first_line(out_snapshot->version);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK || exit_code != 0) {
            return UMI_STATUS_UNAVAILABLE;
        }
        out_snapshot->validated = 1;
    }
    return UMI_STATUS_OK;
}
