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

/**
 * Represent the project build node snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the project build node registry data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectBuildNodeRegistry UmiProjectBuildNodeRegistry;

/**
 * Initialise project build node registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_build_node_registry_create(UmiProjectBuildNodeRegistry **out_registry);
/**
 * Release or reset state held by project build node registry so the same storage can be
 * reused safely.
 */
void umi_project_build_node_registry_destroy(UmiProjectBuildNodeRegistry *registry);
/**
 * Provide the project build node registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_build_node_registry_upsert(UmiProjectBuildNodeRegistry *registry, const UmiProjectBuildNodeSnapshot *item);
/**
 * Remove project build node registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_build_node_registry_remove(UmiProjectBuildNodeRegistry *registry, const char *id);
/**
 * Find project build node registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_project_build_node_registry_find(const UmiProjectBuildNodeRegistry *registry, const char *id, UmiProjectBuildNodeSnapshot *out_item);
/**
 * Find project build node registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_project_build_node_registry_at(const UmiProjectBuildNodeRegistry *registry, size_t index, UmiProjectBuildNodeSnapshot *out_item);
/**
 * Return the number of records represented by project build node registry without changing
 * their state.
 */
size_t umi_project_build_node_registry_count(const UmiProjectBuildNodeRegistry *registry);
/**
 * Provide the project build node registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_project_build_node_registry_revision(const UmiProjectBuildNodeRegistry *registry);
/**
 * Release or reset state held by project build node registry so the same storage can be
 * reused safely.
 */
void umi_project_build_node_registry_clear(UmiProjectBuildNodeRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
