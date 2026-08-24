/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/operation/operation_types.c
 *
 * PURPOSE:
 *   Implement operation names and repository-operation classification.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/operation_types.h"

const char *umi_toolchain_operation_kind_text(UmiToolchainOperationKind kind)
{
    switch (kind) {
        case UMI_TOOLCHAIN_OPERATION_REPOSITORY_READ: return "repository-read";
        case UMI_TOOLCHAIN_OPERATION_REPOSITORY_WRITE: return "repository-write";
        case UMI_TOOLCHAIN_OPERATION_REPOSITORY_REMOTE: return "repository-remote";
        case UMI_TOOLCHAIN_OPERATION_SCAFFOLD_LOCAL: return "scaffold-local";
        case UMI_TOOLCHAIN_OPERATION_SCAFFOLD_REMOTE: return "scaffold-remote";
        case UMI_TOOLCHAIN_OPERATION_BUILD_CONFIGURE: return "build-configure";
        case UMI_TOOLCHAIN_OPERATION_BUILD_COMPILE: return "build-compile";
        case UMI_TOOLCHAIN_OPERATION_BUILD_TEST: return "build-test";
        case UMI_TOOLCHAIN_OPERATION_RUN: return "run";
        case UMI_TOOLCHAIN_OPERATION_DEBUG: return "debug";
        case UMI_TOOLCHAIN_OPERATION_LANGUAGE: return "language";
        case UMI_TOOLCHAIN_OPERATION_FORMAT: return "format";
        case UMI_TOOLCHAIN_OPERATION_PACKAGE: return "package";
        default: return "unknown";
    }
}

int umi_toolchain_operation_is_repository(UmiToolchainOperationKind kind)
{
    return kind == UMI_TOOLCHAIN_OPERATION_REPOSITORY_READ ||
           kind == UMI_TOOLCHAIN_OPERATION_REPOSITORY_WRITE ||
           kind == UMI_TOOLCHAIN_OPERATION_REPOSITORY_REMOTE;
}
