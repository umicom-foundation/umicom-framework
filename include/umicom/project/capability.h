/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/capability.h
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
#ifndef UMICOM_PROJECT_CAPABILITY_H
#define UMICOM_PROJECT_CAPABILITY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_CAPABILITY_CAPACITY 1024U
#define UMI_PROJECT_CAPABILITY_API_VERSION 1U

typedef struct UmiProjectCapabilitySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char capability_id[128];
    char version[128];
    int required;
    int available;
    uint64_t revision;
} UmiProjectCapabilitySnapshot;

typedef struct UmiProjectCapabilityRegistry UmiProjectCapabilityRegistry;

UmiStatus umi_project_capability_registry_create(UmiProjectCapabilityRegistry **out_registry);
void umi_project_capability_registry_destroy(UmiProjectCapabilityRegistry *registry);
UmiStatus umi_project_capability_registry_upsert(UmiProjectCapabilityRegistry *registry, const UmiProjectCapabilitySnapshot *item);
UmiStatus umi_project_capability_registry_remove(UmiProjectCapabilityRegistry *registry, const char *id);
UmiStatus umi_project_capability_registry_find(const UmiProjectCapabilityRegistry *registry, const char *id, UmiProjectCapabilitySnapshot *out_item);
UmiStatus umi_project_capability_registry_at(const UmiProjectCapabilityRegistry *registry, size_t index, UmiProjectCapabilitySnapshot *out_item);
size_t umi_project_capability_registry_count(const UmiProjectCapabilityRegistry *registry);
uint64_t umi_project_capability_registry_revision(const UmiProjectCapabilityRegistry *registry);
void umi_project_capability_registry_clear(UmiProjectCapabilityRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
