/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/descriptor.h
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

typedef struct UmiProjectDescriptorRegistry UmiProjectDescriptorRegistry;

UmiStatus umi_project_descriptor_registry_create(UmiProjectDescriptorRegistry **out_registry);
void umi_project_descriptor_registry_destroy(UmiProjectDescriptorRegistry *registry);
UmiStatus umi_project_descriptor_registry_upsert(UmiProjectDescriptorRegistry *registry, const UmiProjectDescriptorSnapshot *item);
UmiStatus umi_project_descriptor_registry_remove(UmiProjectDescriptorRegistry *registry, const char *id);
UmiStatus umi_project_descriptor_registry_find(const UmiProjectDescriptorRegistry *registry, const char *id, UmiProjectDescriptorSnapshot *out_item);
UmiStatus umi_project_descriptor_registry_at(const UmiProjectDescriptorRegistry *registry, size_t index, UmiProjectDescriptorSnapshot *out_item);
size_t umi_project_descriptor_registry_count(const UmiProjectDescriptorRegistry *registry);
uint64_t umi_project_descriptor_registry_revision(const UmiProjectDescriptorRegistry *registry);
void umi_project_descriptor_registry_clear(UmiProjectDescriptorRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
