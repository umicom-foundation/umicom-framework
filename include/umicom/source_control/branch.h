/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/branch.h
 *
 * PURPOSE:
 *   Define a provider-neutral source-control workspace record above the low-level VCS adapter boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_SOURCE_CONTROL_BRANCH_H
#define UMICOM_SOURCE_CONTROL_BRANCH_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_BRANCH_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_BRANCH_API_VERSION 1U

typedef struct UmiSourceControlBranchSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char name[256];
    char upstream[256];
    char head[128];
    int current;
    int remote;
    uint64_t revision;
} UmiSourceControlBranchSnapshot;

typedef struct UmiSourceControlBranchRegistry UmiSourceControlBranchRegistry;

UmiStatus umi_source_control_branch_registry_create(UmiSourceControlBranchRegistry **out_registry);
void umi_source_control_branch_registry_destroy(UmiSourceControlBranchRegistry *registry);
UmiStatus umi_source_control_branch_registry_upsert(UmiSourceControlBranchRegistry *registry, const UmiSourceControlBranchSnapshot *item);
UmiStatus umi_source_control_branch_registry_remove(UmiSourceControlBranchRegistry *registry, const char *id);
UmiStatus umi_source_control_branch_registry_find(const UmiSourceControlBranchRegistry *registry, const char *id, UmiSourceControlBranchSnapshot *out_item);
UmiStatus umi_source_control_branch_registry_at(const UmiSourceControlBranchRegistry *registry, size_t index, UmiSourceControlBranchSnapshot *out_item);
size_t umi_source_control_branch_registry_count(const UmiSourceControlBranchRegistry *registry);
uint64_t umi_source_control_branch_registry_revision(const UmiSourceControlBranchRegistry *registry);
void umi_source_control_branch_registry_clear(UmiSourceControlBranchRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
