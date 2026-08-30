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

/* BEGINNER NOTE:
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

typedef struct UmiProjectEnvironmentRegistry UmiProjectEnvironmentRegistry;

UmiStatus umi_project_environment_registry_create(UmiProjectEnvironmentRegistry **out_registry);
void umi_project_environment_registry_destroy(UmiProjectEnvironmentRegistry *registry);
UmiStatus umi_project_environment_registry_upsert(UmiProjectEnvironmentRegistry *registry, const UmiProjectEnvironmentSnapshot *item);
UmiStatus umi_project_environment_registry_remove(UmiProjectEnvironmentRegistry *registry, const char *id);
UmiStatus umi_project_environment_registry_find(const UmiProjectEnvironmentRegistry *registry, const char *id, UmiProjectEnvironmentSnapshot *out_item);
UmiStatus umi_project_environment_registry_at(const UmiProjectEnvironmentRegistry *registry, size_t index, UmiProjectEnvironmentSnapshot *out_item);
size_t umi_project_environment_registry_count(const UmiProjectEnvironmentRegistry *registry);
uint64_t umi_project_environment_registry_revision(const UmiProjectEnvironmentRegistry *registry);
void umi_project_environment_registry_clear(UmiProjectEnvironmentRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
