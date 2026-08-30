/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/types.h
 * PURPOSE:
 *   Shared bounded vocabulary for the Framework-owned project/workspace control
 *   plane. Applications consume these contracts and never own reusable state.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_TYPES_H
#define UMICOM_PROJECT_WORKSPACE_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_PROJECT_WORKSPACE_API_VERSION 1U
#define UMI_PROJECT_WORKSPACE_ID_CAPACITY 128U
#define UMI_PROJECT_WORKSPACE_NAME_CAPACITY 256U
#define UMI_PROJECT_WORKSPACE_TEXT_CAPACITY 512U
#define UMI_PROJECT_WORKSPACE_PATH_CAPACITY 2048U
#define UMI_PROJECT_WORKSPACE_SMALL_CAPACITY 16U
#define UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY 32U
#define UMI_PROJECT_WORKSPACE_JSON_CAPACITY 8192U

    typedef enum UmiProjectWorkspaceState {
        UMI_PROJECT_WORKSPACE_UNKNOWN = 0,
        UMI_PROJECT_WORKSPACE_READY = 1,
        UMI_PROJECT_WORKSPACE_DEGRADED = 2,
        UMI_PROJECT_WORKSPACE_BLOCKED = 3,
        UMI_PROJECT_WORKSPACE_INVALID = 4
    }
    UmiProjectWorkspaceState;

    typedef struct UmiProjectWorkspaceNamedState {
        uint32_t structure_size;
        uint32_t api_version;
        char id[UMI_PROJECT_WORKSPACE_ID_CAPACITY];
        char name[UMI_PROJECT_WORKSPACE_NAME_CAPACITY];
        char detail[UMI_PROJECT_WORKSPACE_TEXT_CAPACITY];
        uint64_t revision;
        uint64_t flags;
        uint32_t priority;
        UmiProjectWorkspaceState state;
        bool enabled;
    }
    UmiProjectWorkspaceNamedState;

    const char *umi_project_workspace_state_text(UmiProjectWorkspaceState state);
    UmiStatus umi_project_workspace_named_state_init(UmiProjectWorkspaceNamedState *value,
    const char *id);
    UmiStatus umi_project_workspace_named_state_validate(const UmiProjectWorkspaceNamedState *value);
    UmiStatus umi_project_workspace_named_state_set_name(UmiProjectWorkspaceNamedState *value,
    const char *name);
    UmiStatus umi_project_workspace_named_state_set_detail(UmiProjectWorkspaceNamedState *value,
    const char *detail);
    UmiStatus umi_project_workspace_named_state_set_state(UmiProjectWorkspaceNamedState *value,
    UmiProjectWorkspaceState state);
    bool umi_project_workspace_named_state_same_identity(const UmiProjectWorkspaceNamedState *left,
    const UmiProjectWorkspaceNamedState *right);
#ifdef __cplusplus
}
#endif
#endif
