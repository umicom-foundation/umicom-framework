/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/change_set.h
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
#ifndef UMICOM_SOURCE_CONTROL_CHANGE_SET_H
#define UMICOM_SOURCE_CONTROL_CHANGE_SET_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_CHANGE_SET_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_CHANGE_SET_API_VERSION 1U

typedef struct UmiSourceControlChangeSetSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char name[256];
    char description[512];
    size_t change_count;
    int active;
    uint64_t revision;
} UmiSourceControlChangeSetSnapshot;

typedef struct UmiSourceControlChangeSetRegistry UmiSourceControlChangeSetRegistry;

UmiStatus umi_source_control_change_set_registry_create(UmiSourceControlChangeSetRegistry **out_registry);
void umi_source_control_change_set_registry_destroy(UmiSourceControlChangeSetRegistry *registry);
UmiStatus umi_source_control_change_set_registry_upsert(UmiSourceControlChangeSetRegistry *registry, const UmiSourceControlChangeSetSnapshot *item);
UmiStatus umi_source_control_change_set_registry_remove(UmiSourceControlChangeSetRegistry *registry, const char *id);
UmiStatus umi_source_control_change_set_registry_find(const UmiSourceControlChangeSetRegistry *registry, const char *id, UmiSourceControlChangeSetSnapshot *out_item);
UmiStatus umi_source_control_change_set_registry_at(const UmiSourceControlChangeSetRegistry *registry, size_t index, UmiSourceControlChangeSetSnapshot *out_item);
size_t umi_source_control_change_set_registry_count(const UmiSourceControlChangeSetRegistry *registry);
uint64_t umi_source_control_change_set_registry_revision(const UmiSourceControlChangeSetRegistry *registry);
void umi_source_control_change_set_registry_clear(UmiSourceControlChangeSetRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
