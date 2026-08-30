/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/types.c
 *
 * PURPOSE:
 *   Implement stable project-kind and build-system diagnostic text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/types.h"

const char *umi_developer_project_kind_text(UmiDeveloperProjectKind kind)
{
    switch (kind) {
        case UMI_DEVELOPER_PROJECT_EXECUTABLE: return "executable";
        case UMI_DEVELOPER_PROJECT_STATIC_LIBRARY: return "static-library";
        case UMI_DEVELOPER_PROJECT_SHARED_LIBRARY: return "shared-library";
        case UMI_DEVELOPER_PROJECT_EXTENSION: return "extension";
        case UMI_DEVELOPER_PROJECT_SCRIPT: return "script";
        case UMI_DEVELOPER_PROJECT_WEB: return "web";
        case UMI_DEVELOPER_PROJECT_META: return "meta";
        default: return "unknown";
    }
}

const char *umi_developer_project_build_system_text(
    UmiDeveloperProjectBuildSystem build_system)
{
    switch (build_system) {
        case UMI_DEVELOPER_PROJECT_BUILD_NONE: return "none";
        case UMI_DEVELOPER_PROJECT_BUILD_CMAKE: return "cmake";
        case UMI_DEVELOPER_PROJECT_BUILD_ZIG: return "zig";
        case UMI_DEVELOPER_PROJECT_BUILD_CARGO: return "cargo";
        case UMI_DEVELOPER_PROJECT_BUILD_SCRIPT: return "script";
        case UMI_DEVELOPER_PROJECT_BUILD_CUSTOM: return "custom";
        default: return "unknown";
    }
}
