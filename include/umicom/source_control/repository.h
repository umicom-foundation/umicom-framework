/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/repository.h
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
#ifndef UMICOM_SOURCE_CONTROL_REPOSITORY_H
#define UMICOM_SOURCE_CONTROL_REPOSITORY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_REPOSITORY_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_REPOSITORY_API_VERSION 1U

typedef struct UmiSourceControlRepositorySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char root_uri[1024];
    char provider[64];
    char branch[256];
    char head[128];
    int clean;
    int detached;
    uint64_t revision;
} UmiSourceControlRepositorySnapshot;

typedef struct UmiSourceControlRepositoryRegistry UmiSourceControlRepositoryRegistry;

UmiStatus umi_source_control_repository_registry_create(UmiSourceControlRepositoryRegistry **out_registry);
void umi_source_control_repository_registry_destroy(UmiSourceControlRepositoryRegistry *registry);
UmiStatus umi_source_control_repository_registry_upsert(UmiSourceControlRepositoryRegistry *registry, const UmiSourceControlRepositorySnapshot *item);
UmiStatus umi_source_control_repository_registry_remove(UmiSourceControlRepositoryRegistry *registry, const char *id);
UmiStatus umi_source_control_repository_registry_find(const UmiSourceControlRepositoryRegistry *registry, const char *id, UmiSourceControlRepositorySnapshot *out_item);
UmiStatus umi_source_control_repository_registry_at(const UmiSourceControlRepositoryRegistry *registry, size_t index, UmiSourceControlRepositorySnapshot *out_item);
size_t umi_source_control_repository_registry_count(const UmiSourceControlRepositoryRegistry *registry);
uint64_t umi_source_control_repository_registry_revision(const UmiSourceControlRepositoryRegistry *registry);
void umi_source_control_repository_registry_clear(UmiSourceControlRepositoryRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
