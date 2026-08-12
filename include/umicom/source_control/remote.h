/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/remote.h
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
#ifndef UMICOM_SOURCE_CONTROL_REMOTE_H
#define UMICOM_SOURCE_CONTROL_REMOTE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_REMOTE_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_REMOTE_API_VERSION 1U

typedef struct UmiSourceControlRemoteSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char name[128];
    char fetch_url[1024];
    char push_url[1024];
    int default_remote;
    uint64_t revision;
} UmiSourceControlRemoteSnapshot;

typedef struct UmiSourceControlRemoteRegistry UmiSourceControlRemoteRegistry;

UmiStatus umi_source_control_remote_registry_create(UmiSourceControlRemoteRegistry **out_registry);
void umi_source_control_remote_registry_destroy(UmiSourceControlRemoteRegistry *registry);
UmiStatus umi_source_control_remote_registry_upsert(UmiSourceControlRemoteRegistry *registry, const UmiSourceControlRemoteSnapshot *item);
UmiStatus umi_source_control_remote_registry_remove(UmiSourceControlRemoteRegistry *registry, const char *id);
UmiStatus umi_source_control_remote_registry_find(const UmiSourceControlRemoteRegistry *registry, const char *id, UmiSourceControlRemoteSnapshot *out_item);
UmiStatus umi_source_control_remote_registry_at(const UmiSourceControlRemoteRegistry *registry, size_t index, UmiSourceControlRemoteSnapshot *out_item);
size_t umi_source_control_remote_registry_count(const UmiSourceControlRemoteRegistry *registry);
uint64_t umi_source_control_remote_registry_revision(const UmiSourceControlRemoteRegistry *registry);
void umi_source_control_remote_registry_clear(UmiSourceControlRemoteRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
