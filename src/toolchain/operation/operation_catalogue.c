/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/operation/operation_catalogue.c
 *
 * PURPOSE:
 *   Implement built-in operation profiles, including Git-only repository scopes.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/operation_catalogue.h"

/* Provide the add operation used by this module and its client applications. */
static UmiStatus add(UmiToolchainOperationProfile *profile, UmiToolKind kind)
{
    return umi_toolchain_requirement_set_add(&profile->requirements, kind, 1);
}

/*
 * Provide the toolchain operation catalogue profile operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_operation_catalogue_profile(
    UmiToolchainOperationKind kind,
    UmiToolchainOperationProfile *out_profile)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    umi_toolchain_operation_profile_init(
        out_profile, kind, umi_toolchain_operation_kind_text(kind),
        umi_toolchain_operation_kind_text(kind));

    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_TOOLCHAIN_OPERATION_REPOSITORY_READ:
        case UMI_TOOLCHAIN_OPERATION_REPOSITORY_WRITE:
            status = add(out_profile, UMI_TOOL_GIT);
            break;
        case UMI_TOOLCHAIN_OPERATION_REPOSITORY_REMOTE:
            status = add(out_profile, UMI_TOOL_GIT);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) status = add(out_profile, UMI_TOOL_GH);
            break;
        case UMI_TOOLCHAIN_OPERATION_SCAFFOLD_LOCAL:
            status = add(out_profile, UMI_TOOL_GIT);
            out_profile->requires_compiler = 1;
            out_profile->run_compile_probe = 1;
            break;
        case UMI_TOOLCHAIN_OPERATION_SCAFFOLD_REMOTE:
            status = add(out_profile, UMI_TOOL_GIT);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) status = add(out_profile, UMI_TOOL_GH);
            out_profile->requires_compiler = 1;
            out_profile->run_compile_probe = 1;
            break;
        case UMI_TOOLCHAIN_OPERATION_BUILD_CONFIGURE:
        case UMI_TOOLCHAIN_OPERATION_BUILD_COMPILE:
            status = add(out_profile, UMI_TOOL_CMAKE);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) status = add(out_profile, UMI_TOOL_NINJA);
            out_profile->requires_compiler = 1;
            out_profile->run_compile_probe = 1;
            break;
        case UMI_TOOLCHAIN_OPERATION_PACKAGE:
            status = add(out_profile, UMI_TOOL_CMAKE);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) status = add(out_profile, UMI_TOOL_CPACK);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) status = add(out_profile, UMI_TOOL_NINJA);
            out_profile->requires_compiler = 1;
            out_profile->run_compile_probe = 1;
            break;
        case UMI_TOOLCHAIN_OPERATION_BUILD_TEST:
            status = add(out_profile, UMI_TOOL_CMAKE);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) status = add(out_profile, UMI_TOOL_CTEST);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) status = add(out_profile, UMI_TOOL_NINJA);
            out_profile->requires_compiler = 1;
            out_profile->run_compile_probe = 1;
            break;
        case UMI_TOOLCHAIN_OPERATION_DEBUG:
            status = add(out_profile, UMI_TOOL_GDB);
            break;
        case UMI_TOOLCHAIN_OPERATION_FORMAT:
            status = add(out_profile, UMI_TOOL_CLANG_FORMAT);
            break;
        case UMI_TOOLCHAIN_OPERATION_RUN:
        case UMI_TOOLCHAIN_OPERATION_LANGUAGE:
            break;
        default:
            return UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_toolchain_operation_profile_validate(out_profile);
}
