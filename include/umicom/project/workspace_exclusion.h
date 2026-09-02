/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace_exclusion.h
 * PURPOSE: Define deterministic workspace scan exclusions.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_EXCLUSION_H
#define UMICOM_PROJECT_WORKSPACE_EXCLUSION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/project/workspace_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_WORKSPACE_EXCLUSION_API_VERSION 1U
#define UMI_PROJECT_WORKSPACE_EXCLUSION_CAPACITY 128U

/**
 * List the named project workspace exclusion kind values accepted by this public contract.
 */
typedef enum UmiProjectWorkspaceExclusionKind {
    UMI_PROJECT_WORKSPACE_EXCLUDE_EXACT_NAME = 1,
    UMI_PROJECT_WORKSPACE_EXCLUDE_PREFIX = 2,
    UMI_PROJECT_WORKSPACE_EXCLUDE_SUFFIX = 3,
    UMI_PROJECT_WORKSPACE_EXCLUDE_PATH_SEGMENT = 4
} UmiProjectWorkspaceExclusionKind;

/**
 * Represent the project workspace exclusion snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiProjectWorkspaceExclusionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char pattern[512];
    UmiProjectWorkspaceExclusionKind kind;
    int enabled;
    uint64_t revision;
} UmiProjectWorkspaceExclusionSnapshot;

/**
 * Provide the project workspace model upsert exclusion operation used by this module and
 * its client applications.
 */
UmiStatus umi_project_workspace_model_upsert_exclusion(
    UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceExclusionSnapshot *exclusion);
/**
 * Return the number of records represented by project workspace model exclusion without
 * changing their state.
 */
size_t umi_project_workspace_model_exclusion_count(
    const UmiProjectWorkspaceModel *model);
/**
 * Find project workspace model exclusion while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_project_workspace_model_exclusion_at(
    const UmiProjectWorkspaceModel *model,
    size_t index,
    UmiProjectWorkspaceExclusionSnapshot *out_exclusion);
/**
 * Provide the project workspace model path excluded operation used by this module and its
 * client applications.
 */
int umi_project_workspace_model_path_excluded(
    const UmiProjectWorkspaceModel *model,
    const char *relative_path,
    int is_directory);

#ifdef __cplusplus
}
#endif
#endif
