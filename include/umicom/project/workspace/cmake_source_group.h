/* Umicom Framework | Cmake Source Group | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_PROJECT_WORKSPACE_CMAKE_SOURCE_GROUP_H
#define UMICOM_PROJECT_WORKSPACE_CMAKE_SOURCE_GROUP_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceCmakeSourceGroup {
        char items[UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY][UMI_PROJECT_WORKSPACE_TEXT_CAPACITY];
        size_t count;
        uint64_t revision;
    }
    UmiProjectWorkspaceCmakeSourceGroup;
    void umi_project_workspace_cmake_source_group_init(UmiProjectWorkspaceCmakeSourceGroup *value);
    UmiStatus umi_project_workspace_cmake_source_group_add(UmiProjectWorkspaceCmakeSourceGroup *value, const char *text);
    bool umi_project_workspace_cmake_source_group_contains(const UmiProjectWorkspaceCmakeSourceGroup *value, const char *text);
    UmiStatus umi_project_workspace_cmake_source_group_remove(UmiProjectWorkspaceCmakeSourceGroup *value, const char *text);
#ifdef __cplusplus
}
#endif
#endif
