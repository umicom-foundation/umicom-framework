/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/build_node.h
 *
 * PURPOSE:
 *   Define a reusable project-system record used by Studio and future Umicom development products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_PROJECT_BUILD_NODE_H
#define UMICOM_PROJECT_BUILD_NODE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_BUILD_NODE_CAPACITY 1024U
#define UMI_PROJECT_BUILD_NODE_API_VERSION 1U

typedef struct UmiProjectBuildNodeSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char target_id[128];
    char label[256];
    char kind[64];
    char depends_on[512];
    int state;
    int32_t order;
    uint64_t revision;
} UmiProjectBuildNodeSnapshot;

typedef struct UmiProjectBuildNodeRegistry UmiProjectBuildNodeRegistry;

UmiStatus umi_project_build_node_registry_create(UmiProjectBuildNodeRegistry **out_registry);
void umi_project_build_node_registry_destroy(UmiProjectBuildNodeRegistry *registry);
UmiStatus umi_project_build_node_registry_upsert(UmiProjectBuildNodeRegistry *registry, const UmiProjectBuildNodeSnapshot *item);
UmiStatus umi_project_build_node_registry_remove(UmiProjectBuildNodeRegistry *registry, const char *id);
UmiStatus umi_project_build_node_registry_find(const UmiProjectBuildNodeRegistry *registry, const char *id, UmiProjectBuildNodeSnapshot *out_item);
UmiStatus umi_project_build_node_registry_at(const UmiProjectBuildNodeRegistry *registry, size_t index, UmiProjectBuildNodeSnapshot *out_item);
size_t umi_project_build_node_registry_count(const UmiProjectBuildNodeRegistry *registry);
uint64_t umi_project_build_node_registry_revision(const UmiProjectBuildNodeRegistry *registry);
void umi_project_build_node_registry_clear(UmiProjectBuildNodeRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
