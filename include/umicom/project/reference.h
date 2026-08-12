/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/reference.h
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
#ifndef UMICOM_PROJECT_REFERENCE_H
#define UMICOM_PROJECT_REFERENCE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_REFERENCE_CAPACITY 1024U
#define UMI_PROJECT_REFERENCE_API_VERSION 1U

typedef struct UmiProjectReferenceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char target_project_id[128];
    char kind[64];
    int required;
    int available;
    uint64_t revision;
} UmiProjectReferenceSnapshot;

typedef struct UmiProjectReferenceRegistry UmiProjectReferenceRegistry;

UmiStatus umi_project_reference_registry_create(UmiProjectReferenceRegistry **out_registry);
void umi_project_reference_registry_destroy(UmiProjectReferenceRegistry *registry);
UmiStatus umi_project_reference_registry_upsert(UmiProjectReferenceRegistry *registry, const UmiProjectReferenceSnapshot *item);
UmiStatus umi_project_reference_registry_remove(UmiProjectReferenceRegistry *registry, const char *id);
UmiStatus umi_project_reference_registry_find(const UmiProjectReferenceRegistry *registry, const char *id, UmiProjectReferenceSnapshot *out_item);
UmiStatus umi_project_reference_registry_at(const UmiProjectReferenceRegistry *registry, size_t index, UmiProjectReferenceSnapshot *out_item);
size_t umi_project_reference_registry_count(const UmiProjectReferenceRegistry *registry);
uint64_t umi_project_reference_registry_revision(const UmiProjectReferenceRegistry *registry);
void umi_project_reference_registry_clear(UmiProjectReferenceRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
