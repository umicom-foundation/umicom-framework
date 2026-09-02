/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/dependency.h
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
#ifndef UMICOM_PROJECT_DEPENDENCY_H
#define UMICOM_PROJECT_DEPENDENCY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_DEPENDENCY_CAPACITY 1024U
#define UMI_PROJECT_DEPENDENCY_API_VERSION 1U

/**
 * Represent the project dependency snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectDependencySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char name[256];
    char version[128];
    char source[512];
    char scope[64];
    int optional;
    int resolved;
    uint64_t revision;
} UmiProjectDependencySnapshot;

/**
 * Represent the project dependency registry data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectDependencyRegistry UmiProjectDependencyRegistry;

/**
 * Initialise project dependency registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_dependency_registry_create(UmiProjectDependencyRegistry **out_registry);
/**
 * Release or reset state held by project dependency registry so the same storage can be
 * reused safely.
 */
void umi_project_dependency_registry_destroy(UmiProjectDependencyRegistry *registry);
/**
 * Provide the project dependency registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_dependency_registry_upsert(UmiProjectDependencyRegistry *registry, const UmiProjectDependencySnapshot *item);
/**
 * Remove project dependency registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_dependency_registry_remove(UmiProjectDependencyRegistry *registry, const char *id);
/**
 * Find project dependency registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_project_dependency_registry_find(const UmiProjectDependencyRegistry *registry, const char *id, UmiProjectDependencySnapshot *out_item);
/**
 * Find project dependency registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_project_dependency_registry_at(const UmiProjectDependencyRegistry *registry, size_t index, UmiProjectDependencySnapshot *out_item);
/**
 * Return the number of records represented by project dependency registry without changing
 * their state.
 */
size_t umi_project_dependency_registry_count(const UmiProjectDependencyRegistry *registry);
/**
 * Provide the project dependency registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_project_dependency_registry_revision(const UmiProjectDependencyRegistry *registry);
/**
 * Release or reset state held by project dependency registry so the same storage can be
 * reused safely.
 */
void umi_project_dependency_registry_clear(UmiProjectDependencyRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
