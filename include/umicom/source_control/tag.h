/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/tag.h
 *
 * PURPOSE:
 *   Define a provider-neutral source-control workspace record above the low-level VCS adapter boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_SOURCE_CONTROL_TAG_H
#define UMICOM_SOURCE_CONTROL_TAG_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_TAG_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_TAG_API_VERSION 1U

typedef struct UmiSourceControlTagSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char name[256];
    char target[128];
    char message[512];
    int annotated;
    uint64_t revision;
} UmiSourceControlTagSnapshot;

typedef struct UmiSourceControlTagRegistry UmiSourceControlTagRegistry;

UmiStatus umi_source_control_tag_registry_create(UmiSourceControlTagRegistry **out_registry);
void umi_source_control_tag_registry_destroy(UmiSourceControlTagRegistry *registry);
UmiStatus umi_source_control_tag_registry_upsert(UmiSourceControlTagRegistry *registry, const UmiSourceControlTagSnapshot *item);
UmiStatus umi_source_control_tag_registry_remove(UmiSourceControlTagRegistry *registry, const char *id);
UmiStatus umi_source_control_tag_registry_find(const UmiSourceControlTagRegistry *registry, const char *id, UmiSourceControlTagSnapshot *out_item);
UmiStatus umi_source_control_tag_registry_at(const UmiSourceControlTagRegistry *registry, size_t index, UmiSourceControlTagSnapshot *out_item);
size_t umi_source_control_tag_registry_count(const UmiSourceControlTagRegistry *registry);
uint64_t umi_source_control_tag_registry_revision(const UmiSourceControlTagRegistry *registry);
void umi_source_control_tag_registry_clear(UmiSourceControlTagRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
