/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/dependency.h
 *
 * PURPOSE:
 *   Define a reusable project-system record used by Studio and future Umicom development products.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef struct UmiProjectDependencyRegistry UmiProjectDependencyRegistry;

UmiStatus umi_project_dependency_registry_create(UmiProjectDependencyRegistry **out_registry);
void umi_project_dependency_registry_destroy(UmiProjectDependencyRegistry *registry);
UmiStatus umi_project_dependency_registry_upsert(UmiProjectDependencyRegistry *registry, const UmiProjectDependencySnapshot *item);
UmiStatus umi_project_dependency_registry_remove(UmiProjectDependencyRegistry *registry, const char *id);
UmiStatus umi_project_dependency_registry_find(const UmiProjectDependencyRegistry *registry, const char *id, UmiProjectDependencySnapshot *out_item);
UmiStatus umi_project_dependency_registry_at(const UmiProjectDependencyRegistry *registry, size_t index, UmiProjectDependencySnapshot *out_item);
size_t umi_project_dependency_registry_count(const UmiProjectDependencyRegistry *registry);
uint64_t umi_project_dependency_registry_revision(const UmiProjectDependencyRegistry *registry);
void umi_project_dependency_registry_clear(UmiProjectDependencyRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
