/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/target.h
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
#ifndef UMICOM_PROJECT_TARGET_H
#define UMICOM_PROJECT_TARGET_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_TARGET_CAPACITY 1024U
#define UMI_PROJECT_TARGET_API_VERSION 1U

typedef struct UmiProjectTargetSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char name[256];
    char kind[64];
    char output_uri[1024];
    int enabled;
    int default_target;
    uint64_t revision;
} UmiProjectTargetSnapshot;

typedef struct UmiProjectTargetRegistry UmiProjectTargetRegistry;

UmiStatus umi_project_target_registry_create(UmiProjectTargetRegistry **out_registry);
void umi_project_target_registry_destroy(UmiProjectTargetRegistry *registry);
UmiStatus umi_project_target_registry_upsert(UmiProjectTargetRegistry *registry, const UmiProjectTargetSnapshot *item);
UmiStatus umi_project_target_registry_remove(UmiProjectTargetRegistry *registry, const char *id);
UmiStatus umi_project_target_registry_find(const UmiProjectTargetRegistry *registry, const char *id, UmiProjectTargetSnapshot *out_item);
UmiStatus umi_project_target_registry_at(const UmiProjectTargetRegistry *registry, size_t index, UmiProjectTargetSnapshot *out_item);
size_t umi_project_target_registry_count(const UmiProjectTargetRegistry *registry);
uint64_t umi_project_target_registry_revision(const UmiProjectTargetRegistry *registry);
void umi_project_target_registry_clear(UmiProjectTargetRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
