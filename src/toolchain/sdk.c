/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/sdk.c
 * PURPOSE: Discover SDK metadata associated with a selected compiler profile.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/sdk.h"

#include <stdio.h>
#include <string.h>

#include "umicom/platform/filesystem.h"

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    (void)snprintf(destination, capacity, "%s", source != NULL ? source : "");
}

/* Provide the add sdk operation used by this module and its client applications. */
static UmiStatus add_sdk(UmiSdkCatalogueSnapshot *catalogue,
                         const char *id,
                         const char *name,
                         UmiSdkKind kind,
                         const char *root,
                         const char *include_leaf,
                         const char *library_leaf)
{
    UmiSdkSnapshot *sdk;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || id == NULL || name == NULL || root == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_TOOLCHAIN_SDK_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    sdk = &catalogue->items[catalogue->count++];
    (void)memset(sdk, 0, sizeof(*sdk));
    sdk->struct_size = (uint32_t)sizeof(*sdk);
    sdk->api_version = UMI_TOOLCHAIN_SDK_API_VERSION;
    copy_text(sdk->id, sizeof(sdk->id), id);
    copy_text(sdk->name, sizeof(sdk->name), name);
    copy_text(sdk->root, sizeof(sdk->root), root);
    sdk->kind = kind;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (include_leaf != NULL)
        (void)umi_fs_join(sdk->include_directory,
                          sizeof(sdk->include_directory), root, include_leaf);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (library_leaf != NULL)
        (void)umi_fs_join(sdk->library_directory,
                          sizeof(sdk->library_directory), root, library_leaf);
    sdk->available = umi_fs_is_directory(root) &&
        (sdk->include_directory[0] == '\0' ||
         umi_fs_is_directory(sdk->include_directory));
    return UMI_STATUS_OK;
}

/*
 * Provide the toolchain sdk discover operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_sdk_discover(
    const UmiToolchainProfile *profile,
    UmiSdkCatalogueSnapshot *out_catalogue)
{
    const char *root;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || out_catalogue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_catalogue, 0, sizeof(*out_catalogue));
    out_catalogue->struct_size = (uint32_t)sizeof(*out_catalogue);
    out_catalogue->api_version = UMI_TOOLCHAIN_SDK_API_VERSION;
    root = profile->prefix_directory[0] != '\0'
        ? profile->prefix_directory : profile->root;
    /* Apply this branch only when its contract condition is satisfied. */
    if (root[0] == '\0') {
#ifdef _WIN32
        root = "C:\\Program Files";
#else
        root = "/usr";
#endif
    }
    /* Select the behaviour associated with the requested command or state value. */
    switch (profile->family) {
        case UMI_TOOLCHAIN_MSYS2_UCRT64:
            status = add_sdk(out_catalogue, "msys2-ucrt64", "MSYS2 UCRT64",
                             UMI_SDK_MSYS2_UCRT64, root, "include", "lib");
            break;
        case UMI_TOOLCHAIN_MSYS2_MINGW64:
            status = add_sdk(out_catalogue, "msys2-mingw64", "MSYS2 MINGW64",
                             UMI_SDK_MSYS2_MINGW64, root, "include", "lib");
            break;
        case UMI_TOOLCHAIN_MSYS2_CLANG64:
            status = add_sdk(out_catalogue, "msys2-clang64", "MSYS2 CLANG64",
                             UMI_SDK_MSYS2_UCRT64, root, "include", "lib");
            break;
        case UMI_TOOLCHAIN_MSVC:
            status = add_sdk(out_catalogue, "msvc", "Microsoft Visual C++",
                             UMI_SDK_MSVC, root, NULL, NULL);
            break;
        case UMI_TOOLCHAIN_POSIX_CLANG:
        case UMI_TOOLCHAIN_POSIX_GCC:
            status = add_sdk(out_catalogue, "posix-system", "POSIX System SDK",
                             UMI_SDK_POSIX, root, "include", "lib");
            break;
        default:
            status = add_sdk(out_catalogue, "unknown", "Unknown SDK",
                             UMI_SDK_UNKNOWN, root, NULL, NULL);
            break;
    }
    return status;
}

/* Provide the sdk kind text operation used by this module and its client applications. */
const char *umi_sdk_kind_text(UmiSdkKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_SDK_MSYS2_UCRT64: return "MSYS2 UCRT64";
        case UMI_SDK_MSYS2_MINGW64: return "MSYS2 MINGW64";
        case UMI_SDK_WINDOWS: return "Windows SDK";
        case UMI_SDK_MSVC: return "Microsoft Visual C++";
        case UMI_SDK_POSIX: return "POSIX System SDK";
        default: return "Unknown SDK";
    }
}
