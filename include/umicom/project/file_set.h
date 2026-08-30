/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/file_set.h
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
#ifndef UMICOM_PROJECT_FILE_SET_H
#define UMICOM_PROJECT_FILE_SET_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_FILE_SET_CAPACITY 1024U
#define UMI_PROJECT_FILE_SET_API_VERSION 1U

typedef struct UmiProjectFileSetSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char name[256];
    char root_uri[1024];
    char include_glob[512];
    char exclude_glob[512];
    size_t file_count;
    int generated;
    uint64_t revision;
} UmiProjectFileSetSnapshot;

typedef struct UmiProjectFileSetRegistry UmiProjectFileSetRegistry;

UmiStatus umi_project_file_set_registry_create(UmiProjectFileSetRegistry **out_registry);
void umi_project_file_set_registry_destroy(UmiProjectFileSetRegistry *registry);
UmiStatus umi_project_file_set_registry_upsert(UmiProjectFileSetRegistry *registry, const UmiProjectFileSetSnapshot *item);
UmiStatus umi_project_file_set_registry_remove(UmiProjectFileSetRegistry *registry, const char *id);
UmiStatus umi_project_file_set_registry_find(const UmiProjectFileSetRegistry *registry, const char *id, UmiProjectFileSetSnapshot *out_item);
UmiStatus umi_project_file_set_registry_at(const UmiProjectFileSetRegistry *registry, size_t index, UmiProjectFileSetSnapshot *out_item);
size_t umi_project_file_set_registry_count(const UmiProjectFileSetRegistry *registry);
uint64_t umi_project_file_set_registry_revision(const UmiProjectFileSetRegistry *registry);
void umi_project_file_set_registry_clear(UmiProjectFileSetRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
