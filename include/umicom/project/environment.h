/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/environment.h
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
#ifndef UMICOM_PROJECT_ENVIRONMENT_H
#define UMICOM_PROJECT_ENVIRONMENT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_ENVIRONMENT_CAPACITY 1024U
#define UMI_PROJECT_ENVIRONMENT_API_VERSION 1U

/**
 * Represent the project environment snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectEnvironmentSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char name[256];
    char toolchain_id[128];
    char path_prefix[1024];
    char variables[2048];
    int inherit_parent;
    uint64_t revision;
} UmiProjectEnvironmentSnapshot;

/**
 * Represent the project environment registry data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectEnvironmentRegistry UmiProjectEnvironmentRegistry;

/**
 * Initialise project environment registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_environment_registry_create(UmiProjectEnvironmentRegistry **out_registry);
/**
 * Release or reset state held by project environment registry so the same storage can be
 * reused safely.
 */
void umi_project_environment_registry_destroy(UmiProjectEnvironmentRegistry *registry);
/**
 * Provide the project environment registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_environment_registry_upsert(UmiProjectEnvironmentRegistry *registry, const UmiProjectEnvironmentSnapshot *item);
/**
 * Remove project environment registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_environment_registry_remove(UmiProjectEnvironmentRegistry *registry, const char *id);
/**
 * Find project environment registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_project_environment_registry_find(const UmiProjectEnvironmentRegistry *registry, const char *id, UmiProjectEnvironmentSnapshot *out_item);
/**
 * Find project environment registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_project_environment_registry_at(const UmiProjectEnvironmentRegistry *registry, size_t index, UmiProjectEnvironmentSnapshot *out_item);
/**
 * Return the number of records represented by project environment registry without
 * changing their state.
 */
size_t umi_project_environment_registry_count(const UmiProjectEnvironmentRegistry *registry);
/**
 * Provide the project environment registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_project_environment_registry_revision(const UmiProjectEnvironmentRegistry *registry);
/**
 * Release or reset state held by project environment registry so the same storage can be
 * reused safely.
 */
void umi_project_environment_registry_clear(UmiProjectEnvironmentRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
