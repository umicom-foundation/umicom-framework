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

/*
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

/**
 * Represent the project file set snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the project file set registry data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectFileSetRegistry UmiProjectFileSetRegistry;

/**
 * Initialise project file set registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_file_set_registry_create(UmiProjectFileSetRegistry **out_registry);
/**
 * Release or reset state held by project file set registry so the same storage can be
 * reused safely.
 */
void umi_project_file_set_registry_destroy(UmiProjectFileSetRegistry *registry);
/**
 * Provide the project file set registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_file_set_registry_upsert(UmiProjectFileSetRegistry *registry, const UmiProjectFileSetSnapshot *item);
/**
 * Remove project file set registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_file_set_registry_remove(UmiProjectFileSetRegistry *registry, const char *id);
/**
 * Find project file set registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_file_set_registry_find(const UmiProjectFileSetRegistry *registry, const char *id, UmiProjectFileSetSnapshot *out_item);
/**
 * Find project file set registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_file_set_registry_at(const UmiProjectFileSetRegistry *registry, size_t index, UmiProjectFileSetSnapshot *out_item);
/**
 * Return the number of records represented by project file set registry without changing
 * their state.
 */
size_t umi_project_file_set_registry_count(const UmiProjectFileSetRegistry *registry);
/**
 * Provide the project file set registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_project_file_set_registry_revision(const UmiProjectFileSetRegistry *registry);
/**
 * Release or reset state held by project file set registry so the same storage can be
 * reused safely.
 */
void umi_project_file_set_registry_clear(UmiProjectFileSetRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
