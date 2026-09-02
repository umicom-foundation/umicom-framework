/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/cmake_target_catalogue.h
 *
 * PURPOSE:
 *   Publish the public cmake target catalogue contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_CMAKE_TARGET_CATALOGUE_H
#define UMICOM_PROJECT_WORKSPACE_CMAKE_TARGET_CATALOGUE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace cmake target catalogue data shared with callers of this
 * public contract.
 */
    typedef struct UmiProjectWorkspaceCmakeTargetCatalogue {
        uint32_t structure_size;
        uint32_t api_version;
        char ids[UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY][UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        size_t count;
        uint64_t revision;
    }
    UmiProjectWorkspaceCmakeTargetCatalogue;
    void umi_project_workspace_cmake_target_catalogue_init(UmiProjectWorkspaceCmakeTargetCatalogue *value);
    UmiStatus umi_project_workspace_cmake_target_catalogue_add(UmiProjectWorkspaceCmakeTargetCatalogue *value, const char *id);
    bool umi_project_workspace_cmake_target_catalogue_contains(const UmiProjectWorkspaceCmakeTargetCatalogue *value, const char *id);
    UmiStatus umi_project_workspace_cmake_target_catalogue_at(const UmiProjectWorkspaceCmakeTargetCatalogue *value, size_t index, const char **out_id);
#ifdef __cplusplus
}
#endif
#endif
