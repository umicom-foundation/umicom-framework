/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/change.h
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

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_SOURCE_CONTROL_CHANGE_H
#define UMICOM_SOURCE_CONTROL_CHANGE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_CHANGE_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_CHANGE_API_VERSION 1U

typedef struct UmiSourceControlChangeSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char uri[1024];
    char status[64];
    char old_uri[1024];
    int staged;
    int conflict;
    uint64_t revision;
} UmiSourceControlChangeSnapshot;

typedef struct UmiSourceControlChangeRegistry UmiSourceControlChangeRegistry;

UmiStatus umi_source_control_change_registry_create(UmiSourceControlChangeRegistry **out_registry);
void umi_source_control_change_registry_destroy(UmiSourceControlChangeRegistry *registry);
UmiStatus umi_source_control_change_registry_upsert(UmiSourceControlChangeRegistry *registry, const UmiSourceControlChangeSnapshot *item);
UmiStatus umi_source_control_change_registry_remove(UmiSourceControlChangeRegistry *registry, const char *id);
UmiStatus umi_source_control_change_registry_find(const UmiSourceControlChangeRegistry *registry, const char *id, UmiSourceControlChangeSnapshot *out_item);
UmiStatus umi_source_control_change_registry_at(const UmiSourceControlChangeRegistry *registry, size_t index, UmiSourceControlChangeSnapshot *out_item);
size_t umi_source_control_change_registry_count(const UmiSourceControlChangeRegistry *registry);
uint64_t umi_source_control_change_registry_revision(const UmiSourceControlChangeRegistry *registry);
void umi_source_control_change_registry_clear(UmiSourceControlChangeRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
