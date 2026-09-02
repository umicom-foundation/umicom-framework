/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/descriptor.h
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
#ifndef UMICOM_PROJECT_DESCRIPTOR_H
#define UMICOM_PROJECT_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_DESCRIPTOR_CAPACITY 1024U
#define UMI_PROJECT_DESCRIPTOR_API_VERSION 1U

/**
 * Represent the project descriptor snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectDescriptorSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char name[256];
    char root_uri[1024];
    char kind[64];
    char primary_language[64];
    char version[64];
    char description[512];
    int enabled;
    uint64_t revision;
} UmiProjectDescriptorSnapshot;

/**
 * Represent the project descriptor registry data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectDescriptorRegistry UmiProjectDescriptorRegistry;

/**
 * Initialise project descriptor registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_descriptor_registry_create(UmiProjectDescriptorRegistry **out_registry);
/**
 * Release or reset state held by project descriptor registry so the same storage can be
 * reused safely.
 */
void umi_project_descriptor_registry_destroy(UmiProjectDescriptorRegistry *registry);
/**
 * Provide the project descriptor registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_descriptor_registry_upsert(UmiProjectDescriptorRegistry *registry, const UmiProjectDescriptorSnapshot *item);
/**
 * Remove project descriptor registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_descriptor_registry_remove(UmiProjectDescriptorRegistry *registry, const char *id);
/**
 * Find project descriptor registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_project_descriptor_registry_find(const UmiProjectDescriptorRegistry *registry, const char *id, UmiProjectDescriptorSnapshot *out_item);
/**
 * Find project descriptor registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_project_descriptor_registry_at(const UmiProjectDescriptorRegistry *registry, size_t index, UmiProjectDescriptorSnapshot *out_item);
/**
 * Return the number of records represented by project descriptor registry without changing
 * their state.
 */
size_t umi_project_descriptor_registry_count(const UmiProjectDescriptorRegistry *registry);
/**
 * Provide the project descriptor registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_project_descriptor_registry_revision(const UmiProjectDescriptorRegistry *registry);
/**
 * Release or reset state held by project descriptor registry so the same storage can be
 * reused safely.
 */
void umi_project_descriptor_registry_clear(UmiProjectDescriptorRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
